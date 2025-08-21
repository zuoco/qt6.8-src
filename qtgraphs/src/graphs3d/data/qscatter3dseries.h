// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QTGRAPHS_QSCATTER3DSERIES_H
#define QTGRAPHS_QSCATTER3DSERIES_H

#include <QtGraphs/qabstract3dseries.h>
#include <QtGraphs/qscatterdataproxy.h>

QT_BEGIN_NAMESPACE

class QScatter3DSeriesPrivate;

class Q_GRAPHS_EXPORT QScatter3DSeries : public QAbstract3DSeries
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QScatter3DSeries)
    Q_PROPERTY(QScatterDataProxy *dataProxy READ dataProxy WRITE setDataProxy NOTIFY dataProxyChanged FINAL)
    Q_PROPERTY(
        qsizetype selectedItem READ selectedItem WRITE setSelectedItem NOTIFY selectedItemChanged FINAL)
    Q_PROPERTY(float itemSize READ itemSize WRITE setItemSize NOTIFY itemSizeChanged FINAL)
    Q_PROPERTY(QScatterDataArray dataArray READ dataArray WRITE setDataArray NOTIFY dataArrayChanged FINAL)

public:
    explicit QScatter3DSeries(QObject *parent = nullptr);
    explicit QScatter3DSeries(QScatterDataProxy *dataProxy, QObject *parent = nullptr);
    ~QScatter3DSeries() override;

    void setDataProxy(QScatterDataProxy *proxy);
    QScatterDataProxy *dataProxy() const;

    void setSelectedItem(qsizetype index);
    qsizetype selectedItem() const;
    static qsizetype invalidSelectionIndex();

    void setItemSize(float size);
    float itemSize() const;

    void setDataArray(const QScatterDataArray &newDataArray);
    void clearArray();
    const QScatterDataArray &dataArray() const &;
    QScatterDataArray dataArray() &&;

Q_SIGNALS:
    void dataProxyChanged(QScatterDataProxy *proxy);
    void selectedItemChanged(qsizetype index);
    void itemSizeChanged(float size);
    void dataArrayChanged(const QScatterDataArray &array);

protected:
    explicit QScatter3DSeries(QScatter3DSeriesPrivate &d, QObject *parent = nullptr);

private:
    Q_DISABLE_COPY(QScatter3DSeries)

    friend class QQuickGraphsScatter;
};

QT_END_NAMESPACE

#endif
