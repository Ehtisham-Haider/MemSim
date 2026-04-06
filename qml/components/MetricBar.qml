import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Item {
    id: root
    property string label: ""
    property real value: 0
    property color barColor: Theme.firstFit

    implicitHeight: 28

    RowLayout {
        anchors.fill: parent
        spacing: 8

        Label {
            text: root.label
            color: Theme.textPrimary
            Layout.preferredWidth: 120
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 12
            radius: 6
            color: "#253756"

            Rectangle {
                width: parent.width * Math.max(0, Math.min(1, root.value))
                height: parent.height
                radius: 6
                color: root.barColor
            }
        }

        Label {
            text: Number(root.value).toFixed(2)
            color: Theme.textSecondary
            Layout.preferredWidth: 42
        }
    }
}
