// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QHTTPSERVERSTREAM_P_H
#define QHTTPSERVERSTREAM_P_H

#include <QtCore/qobject.h>

#include <QtHttpServer/qthttpserverglobal.h>
#include <QtHttpServer/qhttpserverresponder.h>
#include <QtHttpServer/qhttpserverrequest.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QHttpServer. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

QT_BEGIN_NAMESPACE

class QTcpSocket;

class QHttpServerStream : public QObject
{
    Q_OBJECT

    friend class QHttpServerResponderPrivate;

protected:
    QHttpServerStream(QObject *parent = nullptr);

    virtual void responderDestroyed() = 0;
    virtual void startHandlingRequest() = 0;
    virtual void socketDisconnected() = 0;

    virtual void write(const QByteArray &body, const QHttpHeaders &headers,
                       QHttpServerResponder::StatusCode status, quint32 streamId) = 0;
    virtual void write(QHttpServerResponder::StatusCode status, quint32 streamId) = 0;
    virtual void write(QIODevice *data, const QHttpHeaders &headers,
                       QHttpServerResponder::StatusCode status, quint32 streamId) = 0;

    virtual void writeBeginChunked(const QHttpHeaders &headers,
                                   QHttpServerResponder::StatusCode status,
                                   quint32 streamId) = 0;
    virtual void writeChunk(const QByteArray &body, quint32 streamId) = 0;
    virtual void writeEndChunked(const QByteArray &data, const
                                 QHttpHeaders &trailers,
                                 quint32 streamId) = 0;

    static QHttpServerRequest initRequestFromSocket(QTcpSocket *socket);
};

QT_END_NAMESPACE

#endif // QHTTPSERVERSTREAM_P_H
