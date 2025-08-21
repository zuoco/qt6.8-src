// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QTGRAPHS_QPIESERIES_H
#define QTGRAPHS_QPIESERIES_H

#include <QtGraphs/qabstractseries.h>
#include <QtGraphs/qgraphsglobal.h>
#include <QtGraphs/qpieslice.h>

QT_BEGIN_NAMESPACE

class QPieSeriesPrivate;

class Q_GRAPHS_EXPORT QPieSeries : public QAbstractSeries
{
    Q_OBJECT
    Q_PROPERTY(qreal horizontalPosition READ horizontalPosition WRITE setHorizontalPosition NOTIFY
                   horizontalPositionChanged FINAL)
    Q_PROPERTY(qreal verticalPosition READ verticalPosition WRITE setVerticalPosition NOTIFY
                   verticalPositionChanged FINAL)
    Q_PROPERTY(qreal pieSize READ pieSize WRITE setPieSize NOTIFY pieSizeChanged FINAL)
    Q_PROPERTY(qreal startAngle READ startAngle WRITE setStartAngle NOTIFY startAngleChanged FINAL)
    Q_PROPERTY(qreal endAngle READ endAngle WRITE setEndAngle NOTIFY endAngleChanged FINAL)
    Q_PROPERTY(qsizetype count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(qreal sum READ sum NOTIFY sumChanged FINAL)
    Q_PROPERTY(qreal holeSize READ holeSize WRITE setHoleSize NOTIFY holeSizeChanged FINAL)
    QML_NAMED_ELEMENT(PieSeries)

public:
    explicit QPieSeries(QObject *parent = nullptr);
    ~QPieSeries() override;
    QAbstractSeries::SeriesType type() const override;

    Q_INVOKABLE bool append(QPieSlice *slice);
    Q_INVOKABLE bool append(const QList<QPieSlice *> &slices);
    Q_INVOKABLE bool insert(qsizetype index, QPieSlice *slice);
    Q_INVOKABLE bool remove(QPieSlice *slice);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QPieSlice *append(const QString &label, qreal value);
    Q_INVOKABLE QPieSlice *at(qsizetype index);
    Q_INVOKABLE QPieSlice *find(const QString &label);
    Q_INVOKABLE bool replace(qsizetype index, QPieSlice *slice);
    Q_INVOKABLE void removeMultiple(qsizetype index, int count);
    Q_INVOKABLE bool remove(qsizetype index);
    Q_INVOKABLE bool replace(QPieSlice *oldSlice, QPieSlice *newSlice);
    Q_INVOKABLE bool replace(const QList<QPieSlice *> &slices);
    Q_INVOKABLE bool take(QPieSlice *slice);

    QPieSeries &operator<<(QPieSlice *slice);

    QList<QPieSlice *> slices() const;
    qsizetype count() const;

    bool isEmpty() const;

    qreal sum() const;

    void setHorizontalPosition(qreal relativePosition);
    qreal horizontalPosition() const;

    void setVerticalPosition(qreal relativePosition);
    qreal verticalPosition() const;
    void setPieSize(qreal relativeSize);
    qreal pieSize() const;

    void setStartAngle(qreal startAngle);
    qreal startAngle() const;

    void setEndAngle(qreal endAngle);
    qreal endAngle() const;

    void setHoleSize(qreal holeSize);
    qreal holeSize() const;

    void setLabelsVisible(bool visible);
    void setLabelsPosition(QPieSlice::LabelPosition position);

public Q_SLOTS:
    void handleSliceChange();

protected:
    QPieSeries(QPieSeriesPrivate &dd, QObject *parent = nullptr);
    void componentComplete() override;

Q_SIGNALS:
    void added(const QList<QPieSlice *> &slices);
    void removed(const QList<QPieSlice *> &slices);
    void replaced(const QList<QPieSlice *> &slices);
    void countChanged();
    void sumChanged();
    void pieSizeChanged();
    void startAngleChanged();
    void endAngleChanged();
    void horizontalPositionChanged();
    void verticalPositionChanged();
    void holeSizeChanged();

private:
    friend class PieRenderer;
    Q_DECLARE_PRIVATE(QPieSeries)
    Q_DISABLE_COPY(QPieSeries)
};

QT_END_NAMESPACE

#endif // QTGRAPHS_QPIESERIES_H
