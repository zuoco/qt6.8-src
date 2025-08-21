// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QTGRAPHS_QVALUEAXIS_H
#define QTGRAPHS_QVALUEAXIS_H

#include <QtGraphs/qabstractaxis.h>
#include <QtGraphs/qgraphsglobal.h>
#include <QtQml/qqmlengine.h>

QT_BEGIN_NAMESPACE

class QValueAxisPrivate;

class Q_GRAPHS_EXPORT QValueAxis : public QAbstractAxis
{
    Q_OBJECT
    Q_PROPERTY(qreal min READ min WRITE setMin NOTIFY minChanged FINAL)
    Q_PROPERTY(qreal max READ max WRITE setMax NOTIFY maxChanged FINAL)
    Q_PROPERTY(
        QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged FINAL)
    Q_PROPERTY(int labelDecimals READ labelDecimals WRITE setLabelDecimals NOTIFY
                   labelDecimalsChanged FINAL)
    Q_PROPERTY(qsizetype subTickCount READ subTickCount WRITE setSubTickCount NOTIFY
                   subTickCountChanged FINAL)
    Q_PROPERTY(qreal tickAnchor READ tickAnchor WRITE setTickAnchor NOTIFY tickAnchorChanged FINAL)
    Q_PROPERTY(
        qreal tickInterval READ tickInterval WRITE setTickInterval NOTIFY tickIntervalChanged FINAL)
    QML_NAMED_ELEMENT(ValueAxis)

public:
    explicit QValueAxis(QObject *parent = nullptr);
    ~QValueAxis() override;

protected:
    QValueAxis(QValueAxisPrivate &d, QObject *parent = nullptr);

public:
    AxisType type() const override;

    //range handling
    void setMin(qreal min);
    qreal min() const;
    void setMax(qreal max);
    qreal max() const;
    void setRange(qreal min, qreal max);

    //ticks handling
    void setSubTickCount(qsizetype count);
    qsizetype subTickCount() const;
    void setTickAnchor(qreal anchor);
    qreal tickAnchor() const;
    void setTickInterval(qreal interval);
    qreal tickInterval() const;

    //label formatting
    void setLabelFormat(const QString &format);
    QString labelFormat() const;
    void setLabelDecimals(int decimals);
    int labelDecimals() const;

Q_SIGNALS:
    void minChanged(qreal min);
    void maxChanged(qreal max);
    void rangeChanged(qreal min, qreal max);
    void subTickCountChanged(qsizetype subTickCount);
    void labelFormatChanged(const QString &format);
    void labelDecimalsChanged(int decimals);
    void tickAnchorChanged(qreal tickAnchor);
    void tickIntervalChanged(qreal tickInterval);

private:
    Q_DECLARE_PRIVATE(QValueAxis)
    Q_DISABLE_COPY(QValueAxis)
};

QT_END_NAMESPACE

#endif // QTGRAPHS_QVALUEAXIS_H
