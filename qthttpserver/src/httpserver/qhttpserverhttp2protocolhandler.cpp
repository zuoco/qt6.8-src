// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qhttpserverhttp2protocolhandler_p.h"

#include <QtCore/qloggingcategory.h>
#include <QtHttpServer/qabstracthttpserver.h>
#include <QtNetwork/private/qhttp2connection_p.h>
#include <QtNetwork/qtcpsocket.h>

#include <private/qhttpserverrequest_p.h>
#include <private/qhttpserverliterals_p.h>
#include <private/qhttpserverresponder_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcHttpServerHttp2Handler, "qt.httpserver.http2handler")

namespace {

void toHeaderPairs(HPack::HttpHeader &fields, const QHttpHeaders &headers)
{
    for (qsizetype i = 0; i < headers.size(); ++i) {
        const auto name = headers.nameAt(i);
        fields.push_back(HPack::HeaderField(QByteArray(name.data(), name.size()),
                                            headers.valueAt(i).toByteArray()));
    }
}

} // anonymous namespace

QHttpServerHttp2ProtocolHandler::QHttpServerHttp2ProtocolHandler(QAbstractHttpServer *server,
                                                                 QIODevice *socket)
    : QHttpServerStream(server),
      m_server(server),
      m_socket(socket),
      m_tcpSocket(qobject_cast<QTcpSocket *>(socket)),
      m_request(QHttpServerStream::initRequestFromSocket(m_tcpSocket))
{
    socket->setParent(this);

    m_connection = QHttp2Connection::createDirectServerConnection(socket,
                                                                  server->http2Configuration());
    if (!m_connection)
        return;

    Q_ASSERT(m_tcpSocket);

    connect(m_tcpSocket,
            &QTcpSocket::readyRead,
            m_connection,
            &QHttp2Connection::handleReadyRead);

    connect(m_tcpSocket,
            &QTcpSocket::disconnected,
            m_connection,
            &QHttp2Connection::handleConnectionClosure);

    connect(m_tcpSocket,
            &QTcpSocket::disconnected,
            this,
            &QHttpServerHttp2ProtocolHandler::socketDisconnected);

    connect(m_connection,
            &QHttp2Connection::newIncomingStream,
            this,
            &QHttpServerHttp2ProtocolHandler::onStreamCreated);
}

void QHttpServerHttp2ProtocolHandler::responderDestroyed()
{
    m_responderCounter--;
}

void QHttpServerHttp2ProtocolHandler::startHandlingRequest()
{
    m_responderCounter++;
}

void QHttpServerHttp2ProtocolHandler::socketDisconnected()
{
    if (m_responderCounter == 0)
        deleteLater();
}

void QHttpServerHttp2ProtocolHandler::write(const QByteArray &body, const QHttpHeaders &headers,
                                            QHttpServerResponder::StatusCode status,
                                            quint32 streamId)
{
    QHttp2Stream *stream = getStream(streamId);
    if (!stream)
        return;

    writeHeadersAndStatus(headers, status, false, streamId);

    QBuffer *buffer = new QBuffer(stream);
    buffer->setData(body);
    buffer->open(QIODevice::ReadOnly);

    connect(stream, &QHttp2Stream::uploadFinished, buffer, &QObject::deleteLater);
    stream->sendDATA(buffer, true);
}

void QHttpServerHttp2ProtocolHandler::write(QHttpServerResponder::StatusCode status,
                                            quint32 streamId)
{
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType,
                   QHttpServerLiterals::contentTypeXEmpty());
    headers.append(QHttpHeaders::WellKnownHeader::ContentLength, "0");

    // RFC 9113, 8.1
    // A HEADERS frame with the END_STREAM flag set that carries
    // an informational status code is malformed
    bool isInfoStatus = QHttpServerResponder::StatusCode::Continue <= status
                        && status < QHttpServerResponder::StatusCode::Ok;
    writeHeadersAndStatus(headers, status, !isInfoStatus, streamId);
}

void QHttpServerHttp2ProtocolHandler::write(QIODevice *data, const QHttpHeaders &headers,
                                  QHttpServerResponder::StatusCode status, quint32 streamId)
{
    QHttp2Stream *stream = getStream(streamId);
    if (!stream)
        return;

    std::unique_ptr<QIODevice, QScopedPointerDeleteLater> input(data);

    if (!input->isOpen()) {
        if (!input->open(QIODevice::ReadOnly)) {
            // TODO Add developer error handling
            qCDebug(lcHttpServerHttp2Handler, "500: Could not open device %ls",
                    qUtf16Printable(input->errorString()));
            write(QHttpServerResponder::StatusCode::InternalServerError, streamId);
            return;
        }
    } else if (!(input->openMode() & QIODevice::ReadOnly)) {
        // TODO Add developer error handling
        qCDebug(lcHttpServerHttp2Handler) << "500: Device is opened in a wrong mode"
                                          << input->openMode();
        write(QHttpServerResponder::StatusCode::InternalServerError, streamId);
        return;
    }

    QHttpHeaders allHeaders(headers);
    if (!data->isSequential()) { // Non-sequential QIODevice should know its data size
        allHeaders.append(QHttpHeaders::WellKnownHeader::ContentLength,
                          QByteArray::number(data->size()));
    }

    writeHeadersAndStatus(headers, status, false, streamId);

    if (input->atEnd()) {
        qCDebug(lcHttpServerHttp2Handler, "No more data available.");
        return;
    }

    input->setParent(stream);
    connect(stream, &QHttp2Stream::uploadFinished, input.get(), &QObject::deleteLater);
    stream->sendDATA(input.release(), true);
}

void QHttpServerHttp2ProtocolHandler::writeBeginChunked(const QHttpHeaders &headers,
                                                        QHttpServerResponder::StatusCode status,
                                                        quint32 streamId)
{
    writeHeadersAndStatus(headers, status, false, streamId);
}

void QHttpServerHttp2ProtocolHandler::writeChunk(const QByteArray &body, quint32 streamId)
{
    enqueueChunk(body, false, {}, streamId);
}

void QHttpServerHttp2ProtocolHandler::writeEndChunked(const QByteArray &body,
                                                      const QHttpHeaders &trailers,
                                                      quint32 streamId)
{
    enqueueChunk(body, true, trailers, streamId);
}

void QHttpServerHttp2ProtocolHandler::enqueueChunk(const QByteArray &body, bool allEnqueued,
                                                   const QHttpHeaders &trailers, quint32 streamId)
{
    QHttp2Stream *stream = getStream(streamId);
    if (!stream)
        return;

    auto &queue = m_streamQueue[streamId];

    if (!trailers.isEmpty()) {
        Q_ASSERT(queue.trailers.empty());
        toHeaderPairs(queue.trailers, trailers);
    }

    queue.data.enqueue(body);
    if (allEnqueued)
        queue.allEnqueued = true;

    if (!stream->isUploadingDATA())
        sendToStream(streamId);
}

void QHttpServerHttp2ProtocolHandler::writeHeadersAndStatus(const QHttpHeaders &headers,
                               QHttpServerResponder::StatusCode status,
                               bool endStream, quint32 streamId)
{
    QHttp2Stream *stream = getStream(streamId);
    if (!stream)
        return;

    HPack::HttpHeader h;
    h.push_back(HPack::HeaderField(":status", QByteArray::number(quint32(status))));
    toHeaderPairs(h, headers);
    stream->sendHEADERS(h, endStream);
}

QHttp2Stream *QHttpServerHttp2ProtocolHandler::getStream(quint32 streamId) const
{
    QHttp2Stream *stream = m_connection->getStream(streamId);
    Q_ASSERT(stream);

    if (stream && stream->isActive())
        return stream;

    return nullptr;
}

void QHttpServerHttp2ProtocolHandler::onStreamCreated(QHttp2Stream *stream)
{
    const quint32 id = stream->streamID();
    m_streamQueue.insert(id, QHttpServerHttp2Queue());

    auto onStateChanged = [this, id](QHttp2Stream::State newState) {
        switch (newState) {
        case QHttp2Stream::State::HalfClosedRemote:
            onStreamHalfClosed(id);
            break;
        case QHttp2Stream::State::Closed:
            onStreamClosed(id);
            break;
        default:
            break;
        }
    };

    auto &connections = m_streamConnections[id];
    connections << connect(stream,
                           &QHttp2Stream::stateChanged,
                           this,
                           onStateChanged,
                           Qt::QueuedConnection);

    connections << connect(stream, &QHttp2Stream::uploadFinished, this,
                           [this, id]() { sendToStream(id); });
}

void QHttpServerHttp2ProtocolHandler::onStreamHalfClosed(quint32 streamId)
{
    auto stream = m_connection->getStream(streamId);
    Q_ASSERT(stream);
    if (!stream)
        return;

    m_request.d->parse(stream);

    qCDebug(lcHttpServerHttp2Handler) << "Request:" << m_request;

    QHttpServerResponder responder(this);
    responder.d_ptr->m_streamId = streamId;

    if (!m_server->handleRequest(m_request, responder))
        m_server->missingHandler(m_request, responder);
}

void QHttpServerHttp2ProtocolHandler::onStreamClosed(quint32 streamId)
{
    auto connections = m_streamConnections.take(streamId);
    for (auto &c : connections)
        disconnect(c);

    m_streamQueue.remove(streamId);
}

void QHttpServerHttp2ProtocolHandler::sendToStream(quint32 streamId)
{
    QHttp2Stream *stream = getStream(streamId);
    if (!stream)
        return;

    if (stream->isUploadingDATA())
        return;

    auto &queue = m_streamQueue[streamId];
    if (!queue.data.isEmpty()) {
        QBuffer *buffer = new QBuffer(stream);
        buffer->setData(queue.data.dequeue());
        buffer->open(QIODevice::ReadOnly);
        connect(stream, &QHttp2Stream::uploadFinished, buffer, &QObject::deleteLater);
        bool endStream = queue.allEnqueued && queue.data.isEmpty() && queue.trailers.empty();
        stream->sendDATA(buffer, endStream);
    } else if (!queue.trailers.empty()) {
        stream->sendHEADERS(queue.trailers, true);
        queue.trailers.clear();
    }
}

QT_END_NAMESPACE
