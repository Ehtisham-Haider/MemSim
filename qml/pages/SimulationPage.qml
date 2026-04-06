import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../style"

Item {
    id: root
    property string selectedAlgorithm: "All"
    property var allStates: []
    property var eventLog: []
    property var currentState: ({})

    signal allocateClicked()
    signal deallocateClicked()

    function selectedState() {
        if (root.allStates.length === 0) return null
        if (root.selectedAlgorithm === "All") return root.allStates[0]
        for (let i = 0; i < root.allStates.length; ++i) {
            if (root.allStates[i].algorithm === root.selectedAlgorithm) return root.allStates[i]
        }
        return root.allStates[0]
    }

    function refreshCurrentState() {
        const state = root.selectedState()
        root.currentState = state ? state : ({})
    }

    onAllStatesChanged: refreshCurrentState()
    onSelectedAlgorithmChanged: {
        refreshCurrentState()
    }
    Component.onCompleted: refreshCurrentState()

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            height: 44
            radius: 8
            color: Theme.panelSurface

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                Label { text: "Algorithm: " + root.selectedAlgorithm; color: Theme.textPrimary; font.bold: true }
                Item { Layout.fillWidth: true }
                Button { text: "+ Allocate"; onClicked: root.allocateClicked() }
                Button { text: "− Deallocate"; onClicked: root.deallocateClicked() }
            }
        }

        MemoryMapView {
            Layout.fillWidth: true
            Layout.preferredHeight: 220
            blocks: root.currentState.blocks || []
            nextFitCursor: (root.currentState && root.selectedAlgorithm === "Next-Fit") ? (root.currentState.nextFitCursor || -1) : -1
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 170
            radius: 8
            color: Theme.panelSurface
            border.color: Theme.deepSurface

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8

                Label { text: "Block Detail Table"; color: Theme.textPrimary; font.bold: true }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        width: parent.width
                        model: root.currentState.blocks || []
                        delegate: RowLayout {
                            width: ListView.view.width
                            spacing: 12
                            Label { text: "BLK-" + modelData.blockId; color: Theme.textPrimary; Layout.preferredWidth: 90 }
                            Label { text: String(modelData.startAddress); color: Theme.textSecondary; Layout.preferredWidth: 90 }
                            Label { text: String(modelData.sizeKB) + " KB"; color: Theme.textSecondary; Layout.preferredWidth: 90 }
                            Label { text: modelData.status; color: modelData.isFree ? Theme.success : Theme.failure; Layout.preferredWidth: 90 }
                            Label { text: modelData.isFree ? "-" : modelData.processLabel; color: Theme.textSecondary; Layout.fillWidth: true }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            FragmentationPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true
                reportData: root.currentState
            }

            EventLogPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true
                eventLog: root.eventLog
            }
        }
    }
}
