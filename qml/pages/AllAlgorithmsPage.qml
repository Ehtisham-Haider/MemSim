import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../style"

Item {
    id: root
    property var allStates: []

    function algorithmColor(name) {
        if (name === "First-Fit") return Theme.firstFit
        if (name === "Next-Fit") return Theme.nextFit
        if (name === "Best-Fit") return Theme.bestFit
        if (name === "Worst-Fit") return Theme.worstFit
        return Theme.textSecondary
    }

    signal runSameRequests()
    signal resetAll()

    GridLayout {
        anchors.fill: parent
        columns: 2
        rowSpacing: 10
        columnSpacing: 10

        Repeater {
            model: root.allStates
            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 8
                color: Theme.panelSurface
                border.color: root.algorithmColor(modelData.algorithm)

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Label { text: modelData.algorithm; color: root.algorithmColor(modelData.algorithm); font.bold: true }
                    MemoryMapView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        blocks: modelData.blocks
                        nextFitCursor: modelData.algorithm === "Next-Fit" ? modelData.nextFitCursor : -1
                    }
                    Label {
                        text: "Free: " + modelData.totalFreeKB + " KB | Frags: " + modelData.freeBlockCount
                        color: Theme.textSecondary
                    }
                }
            }
        }
    }

    Row {
        spacing: 8
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        Button { text: "Run Same Requests on All ▶"; onClicked: root.runSameRequests() }
        Button { text: "Reset All ⟳"; onClicked: root.resetAll() }
    }
}
