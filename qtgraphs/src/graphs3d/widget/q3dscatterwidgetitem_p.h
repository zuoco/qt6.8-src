// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

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

#ifndef Q3DSCATTERWIDGETITEM_P_H
#define Q3DSCATTERWIDGETITEM_P_H

#include <QtGraphsWidgets/q3dscatterwidgetitem.h>
#include <private/q3dgraphswidgetitem_p.h>

QT_BEGIN_NAMESPACE

class Q_GRAPHSWIDGETS_EXPORT Q3DScatterWidgetItemPrivate : public Q3DGraphsWidgetItemPrivate
{
    Q_DECLARE_PUBLIC(Q3DScatterWidgetItem)
public:
    Q3DScatterWidgetItemPrivate() = default;
};

QT_END_NAMESPACE

#endif // Q3DSCATTERWIDGETITEM_P_H
