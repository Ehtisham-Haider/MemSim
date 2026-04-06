import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    property var blockData: ({})
    property int totalKB: 1024

    function blockColor() {
        if (blockData.isFree) {
            return Theme.freeBlock
        }
        const palette = [Theme.firstFit, Theme.nextFit, Theme.bestFit, Theme.worstFit, "#81C784", "#BA68C8", "#64B5F6", "#FFD54F"]
        const pid = (blockData.processId === undefined || blockData.processId === null) ? 0 : Math.abs(blockData.processId)
        return palette[pid % palette.length]
    }

    width: Math.max(60, (blockData.sizeKB || 0) / totalKB * 900)
    height: 90
    radius: 6
    border.width: 1
    border.color: blockData.isFree ? Theme.firstFit : Theme.textPrimary
    color: blockColor()

    Behavior on width { NumberAnimation { duration: 400 } }
    Behavior on color { ColorAnimation { duration: 300 } }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6

        Label {
            text: "BLK-" + blockData.blockId
            color: Theme.textPrimary
            font.family: "JetBrains Mono"
            font.pixelSize: 11
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: (blockData.sizeKB || 0) + " KB"
            color: Theme.textPrimary
            font.bold: true
            font.pixelSize: 16
        }

        Item { Layout.fillHeight: true }

        Rectangle {
            Layout.alignment: Qt.AlignRight
            radius: 10
            color: blockData.isFree ? "#1b5e20" : "#283593"
            border.color: Theme.textSecondary
            border.width: 1
            implicitHeight: 22
            implicitWidth: chipText.implicitWidth + 12

            Label {
                id: chipText
                anchors.centerIn: parent
                text: blockData.isFree ? "● FREE" : "● " + (blockData.processLabel || "P")
                color: Theme.textPrimary
                font.pixelSize: 10
            }
        }
    }
}
