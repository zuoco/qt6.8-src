// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QTGRAPHS_QABSTRACTAXIS_H
#define QTGRAPHS_QABSTRACTAXIS_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGraphs/qgraphsglobal.h>
#include <QtGui/qcolor.h>
#include <QtGui/qfont.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlengine.h>

QT_BEGIN_NAMESPACE

class QAbstractAxisPrivate;

class Q_GRAPHS_EXPORT QAbstractAxis : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    //visibility
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged FINAL)
    Q_PROPERTY(
        bool lineVisible READ isLineVisible WRITE setLineVisible NOTIFY lineVisibleChanged FINAL)
    //labels
    Q_PROPERTY(bool labelsVisible READ labelsVisible WRITE setLabelsVisible NOTIFY
                   labelsVisibleChanged FINAL)
    Q_PROPERTY(
        qreal labelsAngle READ labelsAngle WRITE setLabelsAngle NOTIFY labelsAngleChanged FINAL)
    Q_PROPERTY(QQmlComponent *labelDelegate READ labelDelegate
                       WRITE setLabelDelegate NOTIFY labelDelegateChanged FINAL)
    //grid
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY
                   gridVisibleChanged FINAL)
    Q_PROPERTY(bool subGridVisible READ isSubGridVisible WRITE setSubGridVisible
                   NOTIFY subGridVisibleChanged FINAL)
    //title
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText NOTIFY titleTextChanged FINAL)
    Q_PROPERTY(QColor titleColor READ titleColor WRITE setTitleColor NOTIFY titleColorChanged FINAL)
    Q_PROPERTY(bool titleVisible READ isTitleVisible WRITE setTitleVisible NOTIFY
                   titleVisibleChanged FINAL)
    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont NOTIFY titleFontChanged FINAL)
    QML_FOREIGN(QAbstractAxis)
    QML_UNCREATABLE("")
    QML_NAMED_ELEMENT(AbstractAxis)
    Q_DECLARE_PRIVATE(QAbstractAxis)

public:
    enum class AxisType {
        Value,
        BarCategory,
        DateTime,
    };
    Q_ENUM(AxisType)

protected:
    explicit QAbstractAxis(QAbstractAxisPrivate &dd, QObject *parent = nullptr);

public:
    ~QAbstractAxis() override;

    virtual AxisType type() const = 0;

    //visibility handling
    bool isVisible() const;
    void setVisible(bool visible = true);
    void show();
    void hide();

    //arrow handling
    bool isLineVisible() const;
    void setLineVisible(bool visible = true);

    //grid handling
    bool isGridVisible() const;
    void setGridVisible(bool visible = true);
    bool isSubGridVisible() const;
    void setSubGridVisible(bool visible = true);

    //labels handling
    bool labelsVisible() const;
    void setLabelsVisible(bool visible = true);
    void setLabelsAngle(qreal angle);
    qreal labelsAngle() const;
    QQmlComponent *labelDelegate() const;
    void setLabelDelegate(QQmlComponent *newLabelDelegate);

    //title handling
    bool isTitleVisible() const;
    void setTitleVisible(bool visible = true);
    void setTitleColor(QColor color);
    QColor titleColor() const;
    void setTitleFont(const QFont &font);
    QFont titleFont() const;
    void setTitleText(const QString &title);
    QString titleText() const;

    //range handling
    void setMin(const QVariant &min);
    void setMax(const QVariant &max);
    void setRange(const QVariant &min, const QVariant &max);

Q_SIGNALS:
    void visibleChanged(bool visible);
    void lineVisibleChanged(bool visible);
    void labelsVisibleChanged(bool visible);
    void labelsAngleChanged(qreal angle);
    void labelDelegateChanged();
    void gridVisibleChanged(bool visible);
    void subGridVisibleChanged(bool visible);
    void titleTextChanged(const QString &title);
    void titleColorChanged(QColor color);
    void titleVisibleChanged(bool visible);
    void titleFontChanged(const QFont &font);
    void update();
    void rangeChanged(qreal min, qreal max);

private:
    friend class QGraphsView;
    Q_DISABLE_COPY(QAbstractAxis)
};

QT_END_NAMESPACE

#endif // QTGRAPHS_QABSTRACTAXIS_H
