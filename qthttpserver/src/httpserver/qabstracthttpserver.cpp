// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtHttpServer/qabstracthttpserver.h>

#include <QtHttpServer/qhttpserverrequest.h>
#include <QtHttpServer/qhttpserverresponder.h>

#include <private/qabstracthttpserver_p.h>
#include <private/qhttpserverhttp1protocolhandler_p.h>
#include <private/qhttpserverrequest_p.h>

#include <QtCore/qloggingcategory.h>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>

#if QT_CONFIG(ssl)
#include <QtNetwork/qsslserver.h>
#endif

#if QT_CONFIG(http) && QT_CONFIG(ssl)
#include <private/qhttpserverhttp2protocolhandler_p.h>
#endif

#include <algorithm>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcHttpServer, "qt.httpserver")

/*!
    \internal
*/
QAbstractHttpServerPrivate::QAbstractHttpServerPrivate()
{
}

/*!
    \internal
*/
void QAbstractHttpServerPrivate::handleNewConnections()
{
    Q_Q(QAbstractHttpServer);

#if QT_CONFIG(ssl) && QT_CONFIG(http)
    if (auto *sslServer = qobject_cast<QSslServer *>(q->sender())) {
        while (auto socket = qobject_cast<QSslSocket *>(sslServer->nextPendingConnection())) {
            if (socket->sslConfiguration().nextNegotiatedProtocol()
                            == QSslConfiguration::ALPNProtocolHTTP2) {
                new QHttpServerHttp2ProtocolHandler(q, socket);
            } else {
                new QHttpServerHttp1ProtocolHandler(q, socket);
            }
        }
        return;
    }
#endif

    auto tcpServer = qobject_cast<QTcpServer *>(q->sender());
    Q_ASSERT(tcpServer);

    while (auto socket = tcpServer->nextPendingConnection())
        new QHttpServerHttp1ProtocolHandler(q, socket);
}

/*!
    \internal
*/
bool QAbstractHttpServerPrivate::verifyThreadAffinity(const QObject *contextObject) const {
    Q_Q(const QAbstractHttpServer);
    if (contextObject && (contextObject->thread() != q->thread())) {
        qCWarning(lcHttpServer, "QAbstractHttpServer: "
                                "the context object must reside in the same thread");
        return false;
    }
    return true;
}


#if QT_CONFIG(localserver)
/*!
    \internal
*/
void QAbstractHttpServerPrivate::handleNewLocalConnections()
{
    Q_Q(QAbstractHttpServer);
    auto localServer = qobject_cast<QLocalServer *>(q->sender());
    Q_ASSERT(localServer);

    while (auto socket = localServer->nextPendingConnection())
        new QHttpServerHttp1ProtocolHandler(q, socket);
}
#endif

/*!
    \class QAbstractHttpServer
    \since 6.4
    \inmodule QtHttpServer
    \brief API to subclass to implement an HTTP server.

    Subclass this class and override handleRequest() and missingHandler() to
    create an HTTP server. Use bind() to start listening to all the incoming
    connections to a server.

    This is a low level API, see \l QHttpServer for a highler level API to
    implement an HTTP server.
*/

/*!
    Creates an instance of QAbstractHttpServer with the parent \a parent.
*/
QAbstractHttpServer::QAbstractHttpServer(QObject *parent)
    : QAbstractHttpServer(*new QAbstractHttpServerPrivate, parent)
{}

/*!
    Destroys an instance of QAbstractHttpServer.
*/
QAbstractHttpServer::~QAbstractHttpServer()
    = default;

/*!
    \internal
*/
QAbstractHttpServer::QAbstractHttpServer(QAbstractHttpServerPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
#if defined(QT_WEBSOCKETS_LIB)
    Q_D(QAbstractHttpServer);
    connect(&d->websocketServer, &QWebSocketServer::newConnection,
            this, &QAbstractHttpServer::newWebSocketConnection);
#endif
}

/*!
    Returns the list of ports this instance of QAbstractHttpServer
    is listening to.

    This function has the same guarantee as QObject::children,
    the latest server added is the last entry in the vector.

    \sa servers()
*/
QList<quint16> QAbstractHttpServer::serverPorts() const
{
    QList<quint16> ports;
    auto children = findChildren<QTcpServer *>();
    ports.reserve(children.size());
    std::transform(children.cbegin(), children.cend(), std::back_inserter(ports),
                   [](const QTcpServer *server) { return server->serverPort(); });
    return ports;
}

/*!
    Bind the HTTP server to given TCP \a server over which
    the transmission happens. It is possible to call this function
    multiple times with different instances of TCP \a server to
    handle multiple connections and ports, for example both SSL and
    non-encrypted connections.

    After calling this function, every _new_ connection will be
    handled and forwarded by the HTTP server.

    It is the user's responsibility to call QTcpServer::listen() on
    the \a server before calling this function. If \a server is not
    listening, nothing will happen and \c false will be returned.

    If successful the \a server will be parented to this HTTP server
    and \c true is returned.

    To allow usage of HTTP 2, bind to a QSslServer where
    QSslConfiguration::setAllowedNextProtocols() has been called with
    the arguments \c {{ QSslConfiguration::ALPNProtocolHTTP2 }}.

    \sa QTcpServer, QTcpServer::listen(), QSslConfiguration::setAllowedNextProtocols()
*/
bool QAbstractHttpServer::bind(QTcpServer *server)
{
    Q_D(QAbstractHttpServer);
    if (!server)
        return false;

    if (!server->isListening()) {
        qCWarning(lcHttpServer) << "The TCP server" << server << "is not listening.";
        return false;
    }
    server->setParent(this);
    QObjectPrivate::connect(server, &QTcpServer::pendingConnectionAvailable, d,
                            &QAbstractHttpServerPrivate::handleNewConnections,
                            Qt::UniqueConnection);
    return true;
}

#if QT_CONFIG(localserver)
/*!
    Bind the HTTP server to given QLocalServer \a server over which
    the transmission happens. It is possible to call this function
    multiple times with different instances of \a server to
    handle multiple connections.

    After calling this function, every _new_ connection will be
    handled and forwarded by the HTTP server.

    It is the user's responsibility to call QLocalServer::listen() on
    the \a server before calling this function. If \a server is not
    listening, nothing will happen and \c false will be returned.

    If the \a server is nullptr false is returned.

    If successful the \a server will be parented to this HTTP server
    and \c true is returned.

    \sa QLocalServer, QLocalServer::listen()
*/
bool QAbstractHttpServer::bind(QLocalServer *server)
{
    Q_D(QAbstractHttpServer);
    if (!server)
        return false;

    if (!server->isListening()) {
        qCWarning(lcHttpServer) << "The local server" << server << "is not listening.";
        return false;
    }
    server->setParent(this);
    QObjectPrivate::connect(server, &QLocalServer::newConnection,
                            d, &QAbstractHttpServerPrivate::handleNewLocalConnections,
                            Qt::UniqueConnection);
    return true;
}
#endif

/*!
    Returns the TCP servers of this HTTP server.

    \sa serverPorts()
 */
QList<QTcpServer *> QAbstractHttpServer::servers() const
{
    return findChildren<QTcpServer *>();
}

#if QT_CONFIG(localserver)
/*!
    Returns the local servers of this HTTP server.

    \sa serverPorts()
 */
QList<QLocalServer *> QAbstractHttpServer::localServers() const
{
    return findChildren<QLocalServer *>();
}
#endif

#if defined(QT_WEBSOCKETS_LIB)
/*!
    \fn QAbstractHttpServer::newWebSocketConnection()
    This signal is emitted every time a new WebSocket connection is
    available.

    \sa hasPendingWebSocketConnections(), nextPendingWebSocketConnection(),
    addWebSocketUpgradeVerifier()
*/

/*!
    Returns \c true if the server has pending WebSocket connections;
    otherwise returns \c false.

    \sa newWebSocketConnection(), nextPendingWebSocketConnection(),
    addWebSocketUpgradeVerifier()
*/
bool QAbstractHttpServer::hasPendingWebSocketConnections() const
{
    Q_D(const QAbstractHttpServer);
    return d->websocketServer.hasPendingConnections();
}

/*!
    Returns the next pending connection as a connected QWebSocket
    object. \nullptr is returned if this function is called when
    there are no pending connections.

    \note The returned QWebSocket object cannot be used from another
    thread.

    \sa newWebSocketConnection(), hasPendingWebSocketConnections(),
    addWebSocketUpgradeVerifier()
*/
std::unique_ptr<QWebSocket> QAbstractHttpServer::nextPendingWebSocketConnection()
{
    Q_D(QAbstractHttpServer);
    return std::unique_ptr<QWebSocket>(d->websocketServer.nextPendingConnection());
}

/*!
    \internal
*/
QHttpServerWebSocketUpgradeResponse
QAbstractHttpServer::verifyWebSocketUpgrade(const QHttpServerRequest &request) const
{
    Q_D(const QAbstractHttpServer);
    QScopedValueRollback guard(d->handlingWebSocketUpgrade, true);
    for (auto &verifier : d->webSocketUpgradeVerifiers) {
        if (verifier.context && verifier.slotObject && d->verifyThreadAffinity(verifier.context)) {
            auto response = QHttpServerWebSocketUpgradeResponse::passToNext();
            void *args[] = { &response, const_cast<QHttpServerRequest *>(&request) };
            verifier.slotObject->call(const_cast<QObject *>(verifier.context.data()), args);
            if (response.type() != QHttpServerWebSocketUpgradeResponse::ResponseType::PassToNext)
                return response;
        }
    }
    return QHttpServerWebSocketUpgradeResponse::passToNext();
}

/*!
    \fn template <typename Handler, QAbstractHttpServer::if_compatible_callable<Handler> = true> void QAbstractHttpServer::addWebSocketUpgradeVerifier(const typename QtPrivate::ContextTypeForFunctor<Handler>::ContextType *context, Handler &&func)

    Adds a callback function \a func that verifies incoming WebSocket
    upgrades using the context object \a context. Upgrade attempts
    succeed if at least one of the registered callback functions returns
    \c Accept and a handler returning \c Deny has not been executed before
    it. If no handlers are registered or all return \c PassToNext,
    missingHandler() is called. The callback functions are executed in the
    order they are registered. The callbacks cannot call
    addWebSocketUpgradeVerifier().

    \note The WebSocket upgrades fail if no callbacks has been registered.
    \note This overload participates in overload resolution only if the
    callback function takes a \c const QHttpServerRequest & as an argument
    and returns a QHttpServerWebSocketUpgradeResponse.

    \code
    server.addWebSocketUpgradeVerifier(
            &server, [](const QHttpServerRequest &request) {
                if (request.url().path() == "/allowed"_L1)
                    return QHttpServerWebSocketUpgradeResponse::accept();
                else
                    return QHttpServerWebSocketUpgradeResponse::passToNext();
            });
    \endcode

    \since 6.8
    \sa QHttpServerRequest, QHttpServerWebSocketUpgradeResponse, hasPendingWebSocketConnections(),
    nextPendingWebSocketConnection(), newWebSocketConnection(), missingHandler()
*/

/*!
    \internal
*/
void QAbstractHttpServer::addWebSocketUpgradeVerifierImpl(const QObject *context,
                                                          QtPrivate::QSlotObjectBase *slotObjRaw)
{
    QtPrivate::SlotObjUniquePtr slotObj{slotObjRaw}; // adopts
    Q_ASSERT(slotObj);
    Q_D(QAbstractHttpServer);
    if (d->handlingWebSocketUpgrade) {
        qWarning("Registering WebSocket upgrade verifiers while handling them is not allowed");
        return;
    }
    d->webSocketUpgradeVerifiers.push_back({context, std::move(slotObj)});
}

#endif

/*!
    \fn QAbstractHttpServer::handleRequest(const QHttpServerRequest &request,
                                           QHttpServerResponder &responder)
    Override this function to handle each incoming \a request, by examining
    the \a request and sending the appropriate response back to \a responder.
    Return \c true if the \a request was handled successfully. If this method
    returns \c false, missingHandler() will be called afterwards.

    This function must move out of \a responder before returning \c true.
*/

/*!
    \fn QAbstractHttpServer::missingHandler(const QHttpServerRequest &request,
                                            QHttpServerResponder &responder)

    Override this function to handle each incoming \a request that was not
    handled by \l handleRequest(). This function is called whenever \l
    handleRequest() returns \c false, or if there is a WebSocket upgrade
    attempt and either there are no connections to newWebSocketConnection() or
    there are no matching WebSocket verifiers. The \a request and \a responder
    parameters are the same as handleRequest() was called with.

    \sa handleRequest(), addWebSocketUpgradeVerifier()
*/

#if QT_CONFIG(ssl)
/*!
    \since 6.8

    Returns server's HTTP/2 configuration parameters.

    \sa setHttp2Configuration()
*/
QHttp2Configuration QAbstractHttpServer::http2Configuration() const
{
    Q_D(const QAbstractHttpServer);
    return d->h2Configuration;
}

/*!
    \since 6.8

    Sets server's HTTP/2 configuration parameters.

    The next HTTP/2 connection will use the given \a configuration.

    \sa http2Configuration()
*/
void QAbstractHttpServer::setHttp2Configuration(const QHttp2Configuration &configuration)
{
    Q_D(QAbstractHttpServer);
    d->h2Configuration = configuration;
}

#endif

QT_END_NAMESPACE

#include "moc_qabstracthttpserver.cpp"
