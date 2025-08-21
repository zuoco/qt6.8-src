
// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
import QtQuick
import QtGraphs
import QtTest

Item {
    width: 400
    height: 300

    PieSeries {
        id: pieSeries
        name: "pie"

        SignalSpy {
            id: addedSpy
            target: pieSeries
            signalName: "added"
        }
        SignalSpy {
            id: removedSpy
            target: pieSeries
            signalName: "removed"
        }
        SignalSpy {
            id: sumChangedSpy
            target: pieSeries
            signalName: "sumChanged"
        }
        SignalSpy {
            id: countChangedSpy
            target: pieSeries
            signalName: "countChanged"
        }
    }

    TestCase {
        name: "tst_qml-qtquicktest PieSeries"

        function test_properties() {
            compare(pieSeries.endAngle, 360)
            compare(pieSeries.holeSize, 0)
            compare(pieSeries.horizontalPosition, 0.5)
            compare(pieSeries.pieSize, 0.7)
            compare(pieSeries.startAngle, 0)
            compare(pieSeries.sum, 0)
            compare(pieSeries.verticalPosition, 0.5)
        }

        function test_sliceproperties() {
            var slice = pieSeries.append("slice", 10)
            compare(slice.angleSpan, 360.0)
            verify(slice.borderColor !== undefined)
            compare(slice.borderWidth, 0)
            verify(slice.color !== undefined)
            compare(slice.explodeDistanceFactor, 0.15)
            compare(slice.exploded, false)
            compare(slice.label, "slice")
            compare(slice.labelArmLengthFactor, 0.15)
            verify(slice.labelColor !== undefined)
            compare(slice.labelFont.bold, false)
            compare(slice.labelPosition, PieSlice.LabelPosition.Outside)
            compare(slice.labelVisible, false)
            compare(slice.percentage, 1.0)
            compare(slice.startAngle, 0.0)
            compare(slice.value, 10.0)

            pieSeries.clear()
        }

        function test_take() {
            var count = 20
            for (var i = 0; i < count; i++)
                pieSeries.append("slice" + i, Math.random() + 0.01)

            verify(pieSeries.take(pieSeries.find("slice" + 5)))
            verify(pieSeries.take(pieSeries.find("slice" + 6)))
            compare(pieSeries.count, 18)
        }

        function test_append() {
            addedSpy.clear()
            countChangedSpy.clear()
            sumChangedSpy.clear()
            var count = 50
            for (var i = 0; i < count; i++)
                pieSeries.append("slice" + i,
                                 Math.random() + 0.01) // Add 0.01 to avoid zero
            compare(addedSpy.count, count)
            compare(countChangedSpy.count, count)
            compare(sumChangedSpy.count, count)
            pieSeries.clear()
        }

        function test_remove() {
            removedSpy.clear()
            countChangedSpy.clear()
            sumChangedSpy.clear()

            var count = 50
            for (var i = 0; i < count; i++)
                pieSeries.append("slice" + i,
                                 Math.random() + 0.01) // Add 0.01 to avoid zero
            for (var j = 0; j < 10; j++)
                pieSeries.remove(pieSeries.at(0))

            compare(removedSpy.count, 10)
            compare(countChangedSpy.count, count + 10)
            compare(sumChangedSpy.count, count + 10)
            compare(pieSeries.count, 40)

            for (var j = 0; j < 10; j++)
                pieSeries.remove(0)

            compare(removedSpy.count, 20)
            compare(countChangedSpy.count, count + 20)
            compare(sumChangedSpy.count, count + 20)
            compare(pieSeries.count, 30)

            pieSeries.removeMultiple(0, 10)
            compare(removedSpy.count, 21)
            compare(countChangedSpy.count, count + 21)
            compare(sumChangedSpy.count, count + 30)
            compare(pieSeries.count, 20)

            pieSeries.clear()
        }

        function test_find_and_at() {
            var count = 50
            for (var i = 0; i < count; i++)
                pieSeries.append("slice" + i,
                                 Math.random() + 0.01) // Add 0.01 to avoid zero
            for (var j = 0; j < count; j++) {
                compare(pieSeries.find("slice" + j).label, "slice" + j)
            }
            pieSeries.clear()
        }
    }
}
