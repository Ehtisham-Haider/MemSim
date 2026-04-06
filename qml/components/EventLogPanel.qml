import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    property var eventLog: []

    radius: 8
    color: Theme.panelSurface
    border.color: Theme.deepSurface

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 6

        Label {
            text: "Event Log"
            color: Theme.textPrimary
            font.bold: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: root.eventLog
            clip: true
            delegate: Rectangle {
                width: ListView.view.width
                height: 30
                color: index % 2 === 0 ? "#1D2A45" : "#16213E"
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: modelData.timestamp + "  " + modelData.algorithm + "  " + modelData.detail
                    color: Theme.textPrimary
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    width: parent.width - 16
                }
            }
        }
    }
}
