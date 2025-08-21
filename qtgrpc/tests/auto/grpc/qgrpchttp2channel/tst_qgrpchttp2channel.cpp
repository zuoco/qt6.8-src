// Copyright (C) 2022 The Qt Company Ltd.
// Copyright (C) 2019 Alexey Edelev <semlanik@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGrpc/QGrpcHttp2Channel>
#include <QtGrpc/QGrpcSerializationFormat>
#include <QtGrpc/QGrpcChannelOptions>

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtProtobuf/QProtobufJsonSerializer>
#include <QtProtobuf/QProtobufSerializer>

#include "testservice_client.grpc.qpb.h"

using namespace QtGrpc;

namespace {
using namespace qtgrpc::tests;
using namespace Qt::StringLiterals;

class DummySerializer : public QAbstractProtobufSerializer
{
public:
    virtual QAbstractProtobufSerializer::Error lastError() const override
    {
        return QAbstractProtobufSerializer::Error::None;
    }

    virtual QString lastErrorString() const override { return {}; }

protected:
    virtual QByteArray serializeMessage(const QProtobufMessage *) const override { return {}; }

    virtual bool deserializeMessage(QProtobufMessage *, QByteArrayView) const override
    {
        return true;
    }
};
} // namespace

class QGrpcHttp2ChannelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void checkMethodsGeneration();
    void attachChannelThreadTest();
    void rpcThreadTest();
    void serializationFormat();
    void serializationFormatWithHeaders();
};

void QGrpcHttp2ChannelTest::checkMethodsGeneration()
{
    // Dummy compile time check of functions generation and interface compatibility
    TestService::Client client;
    QGrpcChannelOptions channelOptions;
    client.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl(), channelOptions));
    SimpleStringMessage request;
    auto reply = client.testMethod(request);
    Q_UNUSED(reply)
}

void QGrpcHttp2ChannelTest::attachChannelThreadTest()
{
    std::shared_ptr<QGrpcHttp2Channel> threadChannel;

    std::unique_ptr<QThread> thread(QThread::create([&] {
        threadChannel = std::make_shared<QGrpcHttp2Channel>(QUrl());
    }));
    thread->start();
    thread->wait();

    TestService::Client client;
    QVERIFY(!client.attachChannel(threadChannel));
    QVERIFY(client.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl())));
}

void QGrpcHttp2ChannelTest::rpcThreadTest()
{
    TestService::Client client;
    client.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl()));

    std::unique_ptr<QThread> thread(QThread::create([&] {
        QVERIFY(!client.testMethod(qtgrpc::tests::SimpleStringMessage()));
        QVERIFY(!client.testMethodServerStream(qtgrpc::tests::SimpleStringMessage()));
        QVERIFY(!client.testMethodClientStream(qtgrpc::tests::SimpleStringMessage()));
        QVERIFY(!client.testMethodBiStream(qtgrpc::tests::SimpleStringMessage()));
    }));
    thread->start();
    thread->wait();

    QVERIFY(client.testMethod(qtgrpc::tests::SimpleStringMessage()));
    QVERIFY(client.testMethodServerStream(qtgrpc::tests::SimpleStringMessage()));
    QVERIFY(client.testMethodClientStream(qtgrpc::tests::SimpleStringMessage()));
    QVERIFY(client.testMethodBiStream(qtgrpc::tests::SimpleStringMessage()));
}

void QGrpcHttp2ChannelTest::serializationFormat()
{
    std::shared_ptr<QAbstractGrpcChannel>
        channel = std::make_shared<QGrpcHttp2Channel>(QUrl("http://localhost:50051",
                                                           QUrl::StrictMode));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{}.setSerializationFormat(SerializationFormat::Json));
    QVERIFY(dynamic_cast<QProtobufJsonSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{}
                               .setSerializationFormat(SerializationFormat::Protobuf));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{}
                               .setSerializationFormat(QGrpcSerializationFormat{ "dummy",
                                                         std::make_shared<DummySerializer>() }));
    QVERIFY(dynamic_cast<DummySerializer *>(channel->serializer().get()) != nullptr);
}

void QGrpcHttp2ChannelTest::serializationFormatWithHeaders()
{
    std::shared_ptr<QAbstractGrpcChannel> channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc"_ba } }));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    // Initialize with various content-type headers
    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+json"_ba } }));
    QVERIFY(dynamic_cast<QProtobufJsonSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba,
                                                "application/grpc+proto"_ba } }));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba,
                                                "application/grpc+unknown"_ba } }));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    // Initialize with the default content-type header and various serialization formats
    channel = std::make_shared<QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                                                  QGrpcChannelOptions{
    }
                                                      .setMetadata({ { "content-type"_ba,
                                                                       "application/grpc"_ba } })
                                                      .setSerializationFormat({}));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc"_ba } })
                               .setSerializationFormat(SerializationFormat::Json));
    QVERIFY(dynamic_cast<QProtobufJsonSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc"_ba } })
                               .setSerializationFormat(SerializationFormat::Protobuf));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc"_ba } })
                               .setSerializationFormat(QGrpcSerializationFormat{ "dummy",
                                                         std::make_shared<DummySerializer>() }));
    QVERIFY(dynamic_cast<DummySerializer *>(channel->serializer().get()) != nullptr);

    // Initialize with the content-type header incompatible with serialization format
    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+json"_ba } })
                               .setSerializationFormat(SerializationFormat::Protobuf));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+json"_ba } })
                               .setSerializationFormat({}));
    QVERIFY(dynamic_cast<QProtobufJsonSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+json"_ba } })
                               .setSerializationFormat(QGrpcSerializationFormat{ "dummy",
                                                         std::make_shared<DummySerializer>() }));
    QVERIFY(dynamic_cast<DummySerializer *>(channel->serializer().get()) != nullptr);

    // Initialize with the content-type header matching the serialization format
    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+proto"_ba } })
                               .setSerializationFormat(SerializationFormat::Protobuf));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+json"_ba } })
                               .setSerializationFormat(SerializationFormat::Json));
    QVERIFY(dynamic_cast<QProtobufJsonSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                                                  QGrpcChannelOptions{
    }
                                                      .setMetadata({ { "content-type"_ba,
                                                                       "application/grpc"_ba } })
                                                      .setSerializationFormat({}));
    QVERIFY(dynamic_cast<QProtobufSerializer *>(channel->serializer().get()) != nullptr);

    channel = std::make_shared<
        QGrpcHttp2Channel>(QUrl("http://localhost:50051", QUrl::StrictMode),
                           QGrpcChannelOptions{
    }
                               .setMetadata({ { "content-type"_ba, "application/grpc+dummy"_ba } })
                               .setSerializationFormat(QGrpcSerializationFormat{ "dummy",
                                                         std::make_shared<DummySerializer>() }));
    QVERIFY(dynamic_cast<DummySerializer *>(channel->serializer().get()) != nullptr);
}

QTEST_MAIN(QGrpcHttp2ChannelTest)

#include "tst_qgrpchttp2channel.moc"
