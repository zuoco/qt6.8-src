// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QGRPCHANNELOPTIONS_H
#define QGRPCHANNELOPTIONS_H

#include <QtGrpc/qtgrpcglobal.h>

#if QT_CONFIG(ssl)
#  include <QtNetwork/qsslconfiguration.h>
#endif

#include <QtCore/qhash.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qstringfwd.h>
#include <QtCore/qtclasshelpermacros.h>
#include <QtCore/qurl.h>

#include <chrono>
#include <optional>

QT_BEGIN_NAMESPACE

class QDebug;
class QVariant;
class QGrpcSerializationFormat;

class QGrpcChannelOptionsPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR(QGrpcChannelOptionsPrivate)

class QGrpcChannelOptions final
{
public:
    Q_GRPC_EXPORT QGrpcChannelOptions();
    Q_GRPC_EXPORT ~QGrpcChannelOptions();

    Q_GRPC_EXPORT QGrpcChannelOptions(const QGrpcChannelOptions &other);
    Q_GRPC_EXPORT QGrpcChannelOptions &operator=(const QGrpcChannelOptions &other);

    QGrpcChannelOptions(QGrpcChannelOptions &&other) noexcept = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGrpcChannelOptions)

    Q_GRPC_EXPORT Q_IMPLICIT operator QVariant() const;

    void swap(QGrpcChannelOptions &other) noexcept { d_ptr.swap(other.d_ptr); }

    [[nodiscard]] Q_GRPC_EXPORT std::optional<std::chrono::milliseconds>
    deadlineTimeout() const noexcept;
    Q_GRPC_EXPORT QGrpcChannelOptions &setDeadlineTimeout(std::chrono::milliseconds timeout);

    [[nodiscard]] Q_GRPC_EXPORT const QHash<QByteArray, QByteArray> &metadata() const & noexcept;
    [[nodiscard]] Q_GRPC_EXPORT QHash<QByteArray, QByteArray> metadata() &&;
    Q_GRPC_EXPORT QGrpcChannelOptions &setMetadata(const QHash<QByteArray, QByteArray> &metadata);
    Q_GRPC_EXPORT QGrpcChannelOptions &setMetadata(QHash<QByteArray, QByteArray> &&metadata);

    [[nodiscard]] Q_GRPC_EXPORT QGrpcSerializationFormat serializationFormat() const;
    Q_GRPC_EXPORT QGrpcChannelOptions &
    setSerializationFormat(const QGrpcSerializationFormat &format);

#if QT_CONFIG(ssl)
    [[nodiscard]] Q_GRPC_EXPORT std::optional<QSslConfiguration> sslConfiguration() const;
    Q_GRPC_EXPORT QGrpcChannelOptions &
    setSslConfiguration(const QSslConfiguration &sslConfiguration);
#endif

private:
    QExplicitlySharedDataPointer<QGrpcChannelOptionsPrivate> d_ptr;

#ifndef QT_NO_DEBUG_STREAM
    friend Q_GRPC_EXPORT QDebug operator<<(QDebug debug, const QGrpcChannelOptions &chOpts);
#endif

    Q_DECLARE_PRIVATE(QGrpcChannelOptions)
};

Q_DECLARE_SHARED(QGrpcChannelOptions)

QT_END_NAMESPACE

#endif // QGRPCHANNELOPTIONS_H
