// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtCore/QMetaMethod>
#include "qquickgraphssurfaceseries_p.h"
#include "utils_p.h"

QT_BEGIN_NAMESPACE

QQuickGraphsSurface3DSeries::QQuickGraphsSurface3DSeries(QObject *parent)
    : QSurface3DSeries(parent)

{
    QObject::connect(this,
                     &QSurface3DSeries::selectedPointChanged,
                     this,
                     &QQuickGraphsSurface3DSeries::selectedPointChanged);
}

QQuickGraphsSurface3DSeries::~QQuickGraphsSurface3DSeries() {}

void QQuickGraphsSurface3DSeries::setSelectedPoint(QPointF position)
{
    QSurface3DSeries::setSelectedPoint(position.toPoint());
}

QPointF QQuickGraphsSurface3DSeries::selectedPoint() const
{
    return QPointF(QSurface3DSeries::selectedPoint());
}

QPointF QQuickGraphsSurface3DSeries::invalidSelectionPosition() const
{
    return QPointF(QSurface3DSeries::invalidSelectionPosition());
}

QQmlListProperty<QObject> QQuickGraphsSurface3DSeries::seriesChildren()
{
    return QQmlListProperty<QObject>(this,
                                     this,
                                     &QQuickGraphsSurface3DSeries::appendSeriesChildren,
                                     0,
                                     0,
                                     0);
}

void QQuickGraphsSurface3DSeries::appendSeriesChildren(QQmlListProperty<QObject> *list,
                                                       QObject *element)
{
    QSurfaceDataProxy *proxy = qobject_cast<QSurfaceDataProxy *>(element);
    if (proxy)
        reinterpret_cast<QQuickGraphsSurface3DSeries *>(list->data)->setDataProxy(proxy);
}

void QQuickGraphsSurface3DSeries::setBaseGradient(QQuickGradient *gradient)
{
    if (m_gradients.m_baseGradient != gradient) {
        setGradientHelper(gradient, m_gradients.m_baseGradient, GradientType::Base);
        m_gradients.m_baseGradient = gradient;
        Q_EMIT baseGradientChanged(m_gradients.m_baseGradient);
    }
}

QQuickGradient *QQuickGraphsSurface3DSeries::baseGradient() const
{
    return m_gradients.m_baseGradient;
}

void QQuickGraphsSurface3DSeries::setSingleHighlightGradient(QQuickGradient *gradient)
{
    if (m_gradients.m_singleHighlightGradient != gradient) {
        setGradientHelper(gradient, m_gradients.m_singleHighlightGradient, GradientType::Single);
        m_gradients.m_singleHighlightGradient = gradient;
        Q_EMIT singleHighlightGradientChanged(m_gradients.m_singleHighlightGradient);
    }
}

QQuickGradient *QQuickGraphsSurface3DSeries::singleHighlightGradient() const
{
    return m_gradients.m_singleHighlightGradient;
}

void QQuickGraphsSurface3DSeries::setMultiHighlightGradient(QQuickGradient *gradient)
{
    if (m_gradients.m_multiHighlightGradient != gradient) {
        setGradientHelper(gradient, m_gradients.m_multiHighlightGradient, GradientType::Multi);
        m_gradients.m_multiHighlightGradient = gradient;
        Q_EMIT multiHighlightGradientChanged(m_gradients.m_multiHighlightGradient);
    }
}

QQuickGradient *QQuickGraphsSurface3DSeries::multiHighlightGradient() const
{
    return m_gradients.m_multiHighlightGradient;
}

void QQuickGraphsSurface3DSeries::handleBaseGradientUpdate()
{
    if (!m_gradients.m_baseGradient)
        Utils::setSeriesGradient(this, m_gradients.m_baseGradient, GradientType::Base);
}

void QQuickGraphsSurface3DSeries::handleSingleHighlightGradientUpdate()
{
    if (!m_gradients.m_singleHighlightGradient)
        Utils::setSeriesGradient(this, m_gradients.m_singleHighlightGradient, GradientType::Single);
}

void QQuickGraphsSurface3DSeries::handleMultiHighlightGradientUpdate()
{
    if (!m_gradients.m_multiHighlightGradient)
        Utils::setSeriesGradient(this, m_gradients.m_multiHighlightGradient, GradientType::Multi);
}

void QQuickGraphsSurface3DSeries::setGradientHelper(QQuickGradient *newGradient,
                                                    QQuickGradient *memberGradient,
                                                    GradientType type)
{
    if (memberGradient)
        QObject::disconnect(memberGradient, 0, this, 0);
    Utils::setSeriesGradient(this, newGradient, type);
    memberGradient = newGradient;
    if (memberGradient) {
        switch (type) {
        case GradientType::Base:
            QObject::connect(memberGradient,
                             &QQuickGradient::updated,
                             this,
                             &QQuickGraphsSurface3DSeries::handleBaseGradientUpdate);
            break;
        case GradientType::Single:
            QObject::connect(memberGradient,
                             &QQuickGradient::updated,
                             this,
                             &QQuickGraphsSurface3DSeries::handleSingleHighlightGradientUpdate);
            break;
        case GradientType::Multi:
            QObject::connect(memberGradient,
                             &QQuickGradient::updated,
                             this,
                             &QQuickGraphsSurface3DSeries::handleMultiHighlightGradientUpdate);
            break;
        default:
            break;
        }
    }
}

QT_END_NAMESPACE
