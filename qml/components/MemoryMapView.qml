import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    property var blocks: []
    property int totalKB: 1024
    property int nextFitCursor: -1

    radius: 8
    color: Theme.panelSurface
    border.color: Theme.deepSurface

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        AddressRuler {
            Layout.fillWidth: true
            totalKB: root.totalKB
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Row {
                spacing: 6
                Repeater {
                    model: root.blocks
                    delegate: Item {
                        height: 100
                        width: blockRect.width

                        MemoryBlockRect {
                            id: blockRect
                            anchors.bottom: parent.bottom
                            blockData: modelData
                            totalKB: root.totalKB
                        }

                        Label {
                            visible: root.nextFitCursor === index
                            anchors.horizontalCenter: blockRect.horizontalCenter
                            anchors.top: parent.top
                            text: "▼"
                            color: Theme.nextFit
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }
    }
}
