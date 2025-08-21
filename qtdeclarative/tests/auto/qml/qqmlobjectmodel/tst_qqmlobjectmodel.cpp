// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
#include <QtQmlModels/private/qqmlobjectmodel_p.h>
#include <QtQmlModels/private/qqmlchangeset_p.h>

#include <QtQml/qqmlcomponent.h>

#include <QtTest/qsignalspy.h>
#include <QtTest/qtest.h>

class tst_QQmlObjectModel : public QObject
{
    Q_OBJECT

private slots:
    void changes();
    void objectDestroyed();
};

static bool compareItems(QQmlObjectModel *model, const QObjectList &items)
{
    for (int i = 0; i < items.size(); ++i) {
        if (model->get(i) != items.at(i))
            return false;
    }
    return true;
}

static bool verifyChangeSet(const QQmlChangeSet &changeSet, int expectedInserts, int expectedRemoves, bool isMove, int moveId = -1)
{
    int actualRemoves = 0;
    for (const QQmlChangeSet::Change &r : changeSet.removes()) {
        if (r.isMove() != isMove && (!isMove || moveId == r.moveId))
            return false;
        actualRemoves += r.count;
    }

    int actualInserts = 0;
    for (const QQmlChangeSet::Change &i : changeSet.inserts()) {
        if (i.isMove() != isMove && (!isMove || moveId == i.moveId))
            return false;
        actualInserts += i.count;
    }

    return actualRemoves == expectedRemoves && actualInserts == expectedInserts;
}

Q_DECLARE_METATYPE(QQmlChangeSet)

void tst_QQmlObjectModel::changes()
{
    QQmlObjectModel model;

    qRegisterMetaType<QQmlChangeSet>();

    QSignalSpy countSpy(&model, SIGNAL(countChanged()));
    QSignalSpy childrenSpy(&model, SIGNAL(childrenChanged()));
    QSignalSpy modelUpdateSpy(&model, SIGNAL(modelUpdated(QQmlChangeSet,bool)));

    int count = 0;
    int countSignals = 0;
    int childrenSignals = 0;
    int modelUpdateSignals = 0;

    QObjectList items;
    QObject item0, item1, item2, item3;

    // append(item0) -> [item0]
    model.append(&item0); items.append(&item0);
    QCOMPARE(model.count(), ++count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 1, 0, false));

    // insert(0, item1) -> [item1, item0]
    model.insert(0, &item1); items.insert(0, &item1);
    QCOMPARE(model.count(), ++count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 1, 0, false));

    // append(item2) -> [item1, item0, item2]
    model.append(&item2); items.append(&item2);
    QCOMPARE(model.count(), ++count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 1, 0, false));

    // insert(2, item3) -> [item1, item0, item3, item2]
    model.insert(2, &item3); items.insert(2, &item3);
    QCOMPARE(model.count(), ++count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 1, 0, false));

    // move(0, 1) -> [item0, item1, item3, item2]
    model.move(0, 1); items.move(0, 1);
    QCOMPARE(model.count(), count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 1, 1, true, 1));

    // move(3, 2) -> [item0, item1, item2, item3]
    model.move(3, 2); items.move(3, 2);
    QCOMPARE(model.count(), count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 1, 1, true, 2));

    // remove(0) -> [item1, item2, item3]
    model.remove(0); items.removeAt(0);
    QCOMPARE(model.count(), --count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 0, 1, false));

    // remove(2) -> [item1, item2]
    model.remove(2); items.removeAt(2);
    QCOMPARE(model.count(), --count);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 0, 1, false));

    // clear() -> []
    model.clear(); items.clear();
    QCOMPARE(model.count(), 0);
    QVERIFY(compareItems(&model, items));
    QCOMPARE(countSpy.size(), ++countSignals);
    QCOMPARE(childrenSpy.size(), ++childrenSignals);
    QCOMPARE(modelUpdateSpy.size(), ++modelUpdateSignals);
    QVERIFY(verifyChangeSet(modelUpdateSpy.last().first().value<QQmlChangeSet>(), 0, 2, false));
}

void tst_QQmlObjectModel::objectDestroyed()
{
    QQmlEngine engine;
    QQmlComponent c(&engine);
    c.setData(R"(
        import QtQml
        ObjectModel {
            id: objectModel

            property Component objectComponent: QtObject {}

            Component.onCompleted: {
                objectModel.append(objectComponent.createObject())
                objectModel.get(0).objectName = "first";
            }

            property Timer t: Timer {
                running: true
                interval: 1
                repeat: true
                onTriggered: gc()
            }
        }
    )", QUrl());

    QVERIFY2(c.isReady(), qPrintable(c.errorString()));

    QScopedPointer<QObject> o(c.create());
    QVERIFY(!o.isNull());

    QQmlObjectModel *model = qobject_cast<QQmlObjectModel *>(o.data());
    QVERIFY(model);

    QCOMPARE(model->count(), 1);
    QQmlListProperty<QObject> children = model->children();
    QObject *child = children.at(&children, 0);
    QVERIFY(child);
    QCOMPARE(child->objectName(), QStringLiteral("first"));

    QSignalSpy spy(child, &QObject::destroyed);
    QTRY_COMPARE(spy.count(), 1);

    // Now we should not be able to get to the child anymore
    QCOMPARE(children.at(&children, 0), nullptr);
}

QTEST_MAIN(tst_QQmlObjectModel)

#include "tst_qqmlobjectmodel.moc"
