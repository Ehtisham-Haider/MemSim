import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    property string title: ""
    property string valueText: ""
    property color accent: Theme.firstFit

    radius: 8
    color: Theme.panelSurface
    border.color: accent
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        Label { text: root.title; color: Theme.textSecondary; font.pixelSize: 12 }
        Label { text: root.valueText; color: Theme.textPrimary; font.bold: true; font.pixelSize: 16 }
    }
}
