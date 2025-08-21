// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QGRAPHSVIEW_H
#define QGRAPHSVIEW_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtGraphs API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <QtQuick/QQuickItem>
#include <QtCore/QList>
#include <QtQml/QQmlListProperty>
#include <QtGraphs/qabstractseries.h>
#include <QtGraphs/qgraphstheme.h>

QT_BEGIN_NAMESPACE

class QQuickRectangle;
class QAbstractAxis;
class AxisRenderer;
class BarsRenderer;
class PointRenderer;
class PieRenderer;
class AreaRenderer;

class Q_GRAPHS_EXPORT QGraphsView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QGraphsTheme *theme READ theme WRITE setTheme NOTIFY themeChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QObject> seriesList READ seriesList CONSTANT)
    Q_PROPERTY(qreal marginTop READ marginTop WRITE setMarginTop NOTIFY marginTopChanged FINAL)
    Q_PROPERTY(qreal marginBottom READ marginBottom WRITE setMarginBottom NOTIFY marginBottomChanged FINAL)
    Q_PROPERTY(qreal marginLeft READ marginLeft WRITE setMarginLeft NOTIFY marginLeftChanged FINAL)
    Q_PROPERTY(qreal marginRight READ marginRight WRITE setMarginRight NOTIFY marginRightChanged FINAL)

    Q_PROPERTY(qreal axisXSmoothing READ axisXSmoothing WRITE setAxisXSmoothing NOTIFY axisXSmoothingChanged FINAL)
    Q_PROPERTY(qreal axisYSmoothing READ axisYSmoothing WRITE setAxisYSmoothing NOTIFY axisYSmoothingChanged FINAL)
    Q_PROPERTY(qreal gridSmoothing READ gridSmoothing WRITE setGridSmoothing NOTIFY gridSmoothingChanged FINAL)

    Q_PROPERTY(bool shadowVisible READ isShadowVisible WRITE setShadowVisible NOTIFY
                   shadowVisibleChanged FINAL)
    Q_PROPERTY(QColor shadowColor READ shadowColor WRITE setShadowColor NOTIFY shadowColorChanged FINAL)
    Q_PROPERTY(qreal shadowBarWidth READ shadowBarWidth WRITE setShadowBarWidth NOTIFY shadowBarWidthChanged FINAL)
    Q_PROPERTY(qreal shadowXOffset READ shadowXOffset WRITE setShadowXOffset NOTIFY shadowXOffsetChanged FINAL)
    Q_PROPERTY(qreal shadowYOffset READ shadowYOffset WRITE setShadowYOffset NOTIFY shadowYOffsetChanged FINAL)
    Q_PROPERTY(qreal shadowSmoothing READ shadowSmoothing WRITE setShadowSmoothing NOTIFY shadowSmoothingChanged FINAL)

    Q_PROPERTY(QAbstractAxis *axisX READ axisX WRITE setAxisX NOTIFY axisXChanged FINAL)
    Q_PROPERTY(QAbstractAxis *axisY READ axisY WRITE setAxisY NOTIFY axisYChanged FINAL)
    Q_PROPERTY(
        Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged FINAL)

    Q_CLASSINFO("DefaultProperty", "seriesList")
    QML_NAMED_ELEMENT(GraphsView)

public:
    explicit QGraphsView(QQuickItem *parent = nullptr);
    ~QGraphsView() override;

    Q_INVOKABLE void addSeries(QObject *series);
    Q_INVOKABLE void insertSeries(qsizetype index, QObject *series);
    Q_INVOKABLE void removeSeries(QObject *series);
    Q_INVOKABLE void removeSeries(qsizetype index);
    Q_INVOKABLE bool hasSeries(QObject *series);

    QList<QObject *> getSeriesList() const {
        return m_seriesList;
    }

    QQmlListProperty<QObject> seriesList();
    static void appendSeriesFunc(QQmlListProperty<QObject> *list, QObject *series);
    static qsizetype countSeriesFunc(QQmlListProperty<QObject> *list);
    static QObject *atSeriesFunc(QQmlListProperty<QObject> *list, qsizetype index);
    static void clearSeriesFunc(QQmlListProperty<QObject> *list);

    QGraphsTheme *theme() const;
    void setTheme(QGraphsTheme *newTheme);

    qreal marginTop() const;
    void setMarginTop(qreal newMarginTop);

    qreal marginBottom() const;
    void setMarginBottom(qreal newMarginBottom);

    qreal marginLeft() const;
    void setMarginLeft(qreal newMarginLeft);

    qreal marginRight() const;
    void setMarginRight(qreal newMarginRight);

    QRectF plotArea() const;
    void updatePlotArea();
    void updateAxisAreas();

    void addAxis(QAbstractAxis *axis);
    void removeAxis(QAbstractAxis *axis);

    qsizetype graphSeriesCount() const;
    void setGraphSeriesCount(qsizetype count);

    void createBarsRenderer();
    void createAxisRenderer();
    void createPointRenderer();
    void createPieRenderer();
    void createAreaRenderer();

    qreal axisXSmoothing() const;
    void setAxisXSmoothing(qreal smoothing);
    qreal axisYSmoothing() const;
    void setAxisYSmoothing(qreal smoothing);
    qreal gridSmoothing() const;
    void setGridSmoothing(qreal smoothing);

    bool isShadowVisible() const;
    void setShadowVisible(bool newShadowVisibility);
    QColor shadowColor() const;
    void setShadowColor(QColor newShadowColor);
    qreal shadowBarWidth() const;
    void setShadowBarWidth(qreal newShadowBarWidth);
    qreal shadowXOffset() const;
    void setShadowXOffset(qreal newShadowXOffset);
    qreal shadowYOffset() const;
    void setShadowYOffset(qreal newShadowYOffset);
    qreal shadowSmoothing() const;
    void setShadowSmoothing(qreal smoothing);

    QAbstractAxis *axisX() const;
    void setAxisX(QAbstractAxis *axis);

    QAbstractAxis *axisY() const;
    void setAxisY(QAbstractAxis *axis);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newOrientation);

protected:
    void handleHoverEnter(const QString &seriesName, QPointF position, QPointF value);
    void handleHoverExit(const QString &seriesName, QPointF position);
    void handleHover(const QString &seriesName, QPointF position, QPointF value);
    void updateComponentSizes();
    void componentComplete() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
    void updatePolish() override;

Q_SIGNALS:
    void themeChanged();
    void marginTopChanged();
    void marginBottomChanged();
    void marginLeftChanged();
    void marginRightChanged();
    void hoverEnter(const QString &seriesName, QPointF position, QPointF value);
    void hoverExit(const QString &seriesName, QPointF position);
    void hover(const QString &seriesName, QPointF position, QPointF value);

    void axisXSmoothingChanged();
    void axisYSmoothingChanged();
    void gridSmoothingChanged();

    void shadowVisibleChanged();
    void shadowColorChanged();
    void shadowBarWidthChanged();
    void shadowXOffsetChanged();
    void shadowYOffsetChanged();
    void shadowSmoothingChanged();

    void axisXChanged();
    void axisYChanged();

    void orientationChanged();

private:
    friend class AxisRenderer;
    friend class BarsRenderer;
    friend class PointRenderer;
    friend class AreaRenderer;
    friend class QAbstractAxis;

    void polishAndUpdate();
    int getSeriesRendererIndex(QAbstractSeries *series);

    static constexpr qreal m_defaultAxisTickersWidth = 15;
    static constexpr qreal m_defaultAxisTickersHeight = 15;
    static constexpr qreal m_defaultAxisLabelsWidth = 40;
    static constexpr qreal m_defaultAxisLabelsHeight = 25;
    static constexpr qreal m_defaultAxisXLabelsMargin = 0;
    static constexpr qreal m_defaultAxisYLabelsMargin = 5;

    AxisRenderer *m_axisRenderer = nullptr;
    BarsRenderer *m_barsRenderer = nullptr;
    PointRenderer *m_pointRenderer = nullptr;
    PieRenderer *m_pieRenderer = nullptr;
    AreaRenderer *m_areaRenderer = nullptr;
    QList<QObject *> m_seriesList;
    QHash<int, QList<QAbstractSeries *>> m_cleanupSeriesList;
    QQuickRectangle *m_backgroundRectangle = nullptr;

    QAbstractAxis *m_axisX = nullptr;
    QAbstractAxis *m_axisY = nullptr;
    Qt::Orientation m_orientation = Qt::Orientation::Vertical;

    QGraphsTheme *m_theme = nullptr;
    QGraphsTheme *m_defaultTheme = nullptr;

    qsizetype m_graphSeriesCount = 0;

    qreal m_marginTop = 20;
    qreal m_marginBottom = 20;
    qreal m_marginLeft = 20;
    qreal m_marginRight = 20;
    QRectF m_plotArea;
    // Areas of axis
    QRectF m_xAxisArea;
    QRectF m_yAxisArea;
    // Areas of axis tickers
    QRectF m_xAxisTickersArea;
    QRectF m_yAxisTickersArea;
    // Areas of axis labels
    QRectF m_xAxisLabelsArea;
    QRectF m_yAxisLabelsArea;
    // Note: Add properties for these
    qreal m_axisTickersWidth = m_defaultAxisTickersWidth;
    qreal m_axisTickersHeight = m_defaultAxisTickersHeight;
    qreal m_axisLabelsWidth = m_defaultAxisLabelsWidth;
    qreal m_axisLabelsHeight = m_defaultAxisLabelsHeight;
    qreal m_axisXLabelsMargin = m_defaultAxisXLabelsMargin;
    qreal m_axisYLabelsMargin = m_defaultAxisYLabelsMargin;
    // Calculated based the the above
    qreal m_axisWidth = 0;
    qreal m_axisHeight = 0;

    int m_hoverCount = 0;

    qreal m_axisXSmoothing = 1.0;
    qreal m_axisYSmoothing = 1.0;
    qreal m_gridSmoothing = 1.0;

    bool m_isShadowVisible = false;
    QColor m_shadowColor = QColorConstants::Black;
    qreal m_shadowBarWidth = 2.0;
    qreal m_shadowXOffset = 0.0;
    qreal m_shadowYOffset = 0.0;
    qreal m_shadowSmoothing = 4.0;
};

QT_END_NAMESPACE

#endif
