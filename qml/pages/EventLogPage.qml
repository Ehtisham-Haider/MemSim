import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Item {
    id: root
    property var eventLog: []

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        Rectangle {
            Layout.fillWidth: true
            height: 50
            radius: 8
            color: Theme.panelSurface
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                Label { text: "EVENT LOG"; color: Theme.textPrimary; font.bold: true }
                Item { Layout.fillWidth: true }
                TextField { id: searchField; placeholderText: "Search..." }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: Theme.panelSurface
            border.color: Theme.deepSurface

            ListView {
                anchors.fill: parent
                anchors.margins: 8
                clip: true
                model: root.eventLog
                delegate: Rectangle {
                    width: ListView.view.width
                    height: 32
                    visible: searchField.text.length === 0 || modelData.detail.toLowerCase().indexOf(searchField.text.toLowerCase()) >= 0
                    color: index % 2 ? "#15213A" : "#1A2947"
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 6
                        Label { text: modelData.timestamp; color: Theme.textSecondary; Layout.preferredWidth: 90 }
                        Label { text: modelData.algorithm; color: Theme.textPrimary; Layout.preferredWidth: 90 }
                        Label { text: modelData.detail; color: Theme.textPrimary; Layout.fillWidth: true; elide: Text.ElideRight }
                    }
                }
            }
        }
    }
}
