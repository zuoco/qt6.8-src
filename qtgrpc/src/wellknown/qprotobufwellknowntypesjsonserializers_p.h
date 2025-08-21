// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPROTOBUFWELLKNOWNTYPESJSONSERIALIZERS_P_H
#define QPROTOBUFWELLKNOWNTYPESJSONSERIALIZERS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qtconfigmacros.h>

QT_BEGIN_NAMESPACE

namespace QtProtobufWellKnownTypesPrivate {
void registerTimestampCustomJsonHandler();
void registerDurationCustomJsonHandler();
void registerBoolValueCustomJsonHandler();
void registerInt32ValueCustomJsonHandler();
void registerInt64ValueCustomJsonHandler();
void registerUInt32ValueCustomJsonHandler();
void registerUInt64ValueCustomJsonHandler();
void registerFloatValueCustomJsonHandler();
void registerDoubleValueCustomJsonHandler();
void registerStringValueCustomJsonHandler();
void registerBytesValueCustomJsonHandler();
}

QT_END_NAMESPACE

#endif // QPROTOBUFWELLKNOWNTYPESJSONSERIALIZERS_P_H
