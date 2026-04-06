import QtQuick
import QtQuick.Controls
import "../style"

Item {
    id: root
    property int totalKB: 1024

    height: 24

    Repeater {
        model: Math.floor(root.totalKB / 100) + 1
        delegate: Item {
            x: index * (parent.width / (model - 1))
            width: 1
            height: parent.height

            Rectangle { width: 1; height: 10; color: Theme.textSecondary }
            Label {
                visible: index % 2 === 0
                y: 10
                x: -10
                text: (index * 100)
                color: Theme.textSecondary
                font.pixelSize: 10
            }
        }
    }

    Label {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        text: totalKB + " KB"
        color: Theme.textSecondary
        font.pixelSize: 10
    }
}
