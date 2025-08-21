// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QTGRAPHS_QAREASERIES_H
#define QTGRAPHS_QAREASERIES_H

#include <QtGraphs/qabstractseries.h>
#include <QtGraphs/qgraphsglobal.h>
#include <QtGraphs/qxyseries.h>

QT_BEGIN_NAMESPACE

class QAreaSeriesPrivate;

class Q_GRAPHS_EXPORT QAreaSeries : public QAbstractSeries
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor NOTIFY
                   selectedColorChanged FINAL)
    Q_PROPERTY(
        QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged FINAL)
    Q_PROPERTY(QColor selectedBorderColor READ selectedBorderColor WRITE setSelectedBorderColor
                   NOTIFY selectedBorderColorChanged FINAL)
    Q_PROPERTY(
        qreal borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged FINAL)
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged FINAL)
    Q_PROPERTY(QXYSeries *upperSeries READ upperSeries WRITE setUpperSeries NOTIFY
                   upperSeriesChanged FINAL)
    Q_PROPERTY(QXYSeries *lowerSeries READ lowerSeries WRITE setLowerSeries NOTIFY
                   lowerSeriesChanged FINAL)

    QML_NAMED_ELEMENT(AreaSeries)
public:
    explicit QAreaSeries(QObject *parent = nullptr);
    ~QAreaSeries() override;
    QAbstractSeries::SeriesType type() const override;

    QColor color() const;
    void setColor(QColor newColor);

    QColor selectedColor() const;
    void setSelectedColor(QColor newColor);

    QColor borderColor() const;
    void setBorderColor(QColor newBorderColor);

    QColor selectedBorderColor() const;
    void setSelectedBorderColor(QColor newSelectedBorderColor);

    qreal borderWidth() const;
    void setBorderWidth(qreal newBorderWidth);

    bool isSelected() const;
    void setSelected(bool newSelected);

    QXYSeries *upperSeries() const;
    void setUpperSeries(QXYSeries *newUpperSeries);

    QXYSeries *lowerSeries() const;
    void setLowerSeries(QXYSeries *newLowerSeries);

Q_SIGNALS:
    void colorChanged(QColor newColor);
    void selectedColorChanged(QColor newSelectedColor);
    void borderColorChanged(QColor newBorderColor);
    void selectedBorderColorChanged(QColor newSelectedBorderColor);
    void borderWidthChanged();
    void selectedChanged();
    void upperSeriesChanged();
    void lowerSeriesChanged();

protected:
    QAreaSeries(QAreaSeriesPrivate &dd, QObject *parent = nullptr);

private:
    friend class AreaRenderer;
    Q_DECLARE_PRIVATE(QAreaSeries)
    Q_DISABLE_COPY(QAreaSeries)
};

QT_END_NAMESPACE

#endif // QTGRAPHS_QAREASERIES_H
