// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import QtQuick.Dialogs

Rectangle {
    id: mainView
    width: 1280
    height: 720
    color: Application.styleHints.colorScheme === Qt.Dark ? "darkgray" : "lightgray"

    property var item: stackLayout.itemAt(stackLayout.currentIndex)
    property var outputsize: Qt.size(linegraph.width * 4, linegraph.height * 4)

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        spacing: 5

        GroupBox {
            id: groupBox
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            title: qsTr("Printing and exporting")

            ColumnLayout {
                id: buttonLayout
                spacing: 0
                uniformCellSizes: true
                //! [1.2]
                Button {
                    id: setFolderButton
                    //! [1.2]
                    text: qsTr("Set save location")
                    flat: true

                    icon.source: pressed ? "folder_fill.svg" : "folder.svg"
                    icon.height: 36
                    icon.width: 36
                    //! [1.3]
                    onClicked: dialog.open()
                }
                //! [1.3]
                Button {
                    id: captureButton
                    text: qsTr("Save to PDF")
                    flat: true

                    icon.source: pressed ? "documents_fill.svg" : "documents.svg"
                    icon.height: 36
                    icon.width: 36

                    //! [3.1]
                    onPressed: {
                        if (!dialog.folderset) {
                            message.title = "No Folder Set"
                            message.text = "Please select folder first"
                            message.open()
                        } else {
                            mainView.prepareForPrint()
                            mainView.item.grabToImage(function (result) {
                                message.title = "Save PDF"
                                message.text = "PDF saved to " + graphPrinter.generatePDF(
                                            dialog.currentFolder, result.image)
                                message.open()
                            }, mainView.outputsize)
                        }
                    }

                    onReleased: {
                        mainView.cleanAfterPrint()
                    }
                    //! [3.1]
                }

                Button {
                    id: printButton
                    text: qsTr("Send to printer")
                    flat: true

                    icon.source: pressed ? "print_fill.svg" : "print.svg"
                    icon.height: 36
                    icon.width: 36

                    onPressed: printerDialog.open()
                }
            }
        }

        Item {
            id: tabGroup
            Layout.fillHeight: true
            Layout.fillWidth: true

            //! [0]
            TabBar {
                id: tabBar
                anchors.left: parent.left
                anchors.right: parent.right

                TabButton {
                    text: "2D Graph"
                    implicitHeight: 48
                    icon.source: checked ? "flatten_square_fill.svg" : "flatten.svg"
                    icon.height: 36
                    icon.width: 36
                }

                TabButton {
                    text: "3D Graph"
                    implicitHeight: 48
                    icon.source: checked ? "box_left_fill.svg" : "box_left.svg"
                    icon.height: 36
                    icon.width: 36
                }
            }
            Frame {
                id: tabFrame
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: tabBar.bottom
                anchors.bottom: parent.bottom

                StackLayout {
                    id: stackLayout

                    anchors.fill: parent
                    currentIndex: tabBar.currentIndex

                    Graph2D {
                        id: linegraph
                    }

                    Graph3D {
                        id: bargraph
                    }
                }
            }
            //! [0]
        }
    }
    MessageDialog {
        id: message
    }

    //! [1.1]
    FolderDialog {
        id: dialog
        property bool folderset: false
        onAccepted: {
            folderset = true
            message.title = "Folder Set"
            message.text = "Folder set to " + selectedFolder.toString().replace(/^(file:\/{3})/, "")
            message.open()
        }
    }
    //! [1.1]

    //! [2.1]
    Dialog {
        id: printerDialog
        anchors.centerIn: parent
        contentHeight: printerListView.height
        contentWidth: printerListView.width

        title: qsTr("Available Printers")
        modal: true

        onOpened: {
            printerModel.clear()
            var printers = graphPrinter.getPrinters()
            printers.forEach((x, i) => printerModel.append({
                                                               "name": x
                                                           }))
        }
        //! [2.1]

        //! [3.2]
        onAccepted: {
            var selectedPrinter = printerModel.get(printerListView.currentIndex)
            mainView.prepareForPrint()
            mainView.item.grabToImage(function (result) {
                message.title = "Print"
                message.text = graphPrinter.print(result.image,
                                                  selectedPrinter.name)
                message.open()
            }, mainView.outputsize)
        }

        onClosed: {
            mainView.cleanAfterPrint()
        }

        //! [3.2]
        Component {
            id: printerDelegate
            Rectangle {
                width: 198
                height: 25
                color: "transparent"
                border.color: mainView.item.theme.grid.mainColor
                clip: true

                Text {
                    padding: 5
                    text: qsTr("<b>%1</b>").arg(name)
                    color: mainView.item.theme.labelTextColor
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: printerListView.currentIndex = index
                }
            }
        }

        //! [2.2]
        contentItem: Rectangle {
            id: printerItem
            height: printerListView.height
            width: printerListView.width
            color: mainView.item.theme.plotAreaBackgroundColor

            ListView {
                id: printerListView
                height: 100
                width: 200
                clip: true

                model: printerModel
                delegate: printerDelegate
                highlight: Rectangle {
                    color: mainView.item.theme.grid.subColor
                }
            }
        }

        //! [2.2]
        footer: DialogButtonBox {
            Button {
                text: "Print"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
            standardButtons: Dialog.Cancel
        }
    }

    ListModel {
        id: printerModel
    }

    //! [3.3]
    function prepareForPrint() {
        if (stackLayout.currentIndex === 1) {
            outputsize = Qt.size(bargraph.width * 4, bargraph.height * 4)
            // resize the bar graph to match the PDF output size
            item.width = outputsize.width
            item.height = outputsize.height
        } else {
            outputsize = Qt.size(linegraph.width * 4, linegraph.height * 4)
        }
    }

    function cleanAfterPrint() {
        if (stackLayout.currentIndex === 1) {
            // resize the bar graph back to the actual visual size
            item.width = mainView.width
            item.height = mainView.height
        }
    }
    //! [3.3]
}
