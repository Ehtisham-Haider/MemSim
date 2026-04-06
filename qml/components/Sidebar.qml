import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    color: Theme.panelSurface

    property string selectedAlgorithm: "All"
    property var allStates: []

    function selectedState() {
        if (root.allStates.length === 0) return null
        if (root.selectedAlgorithm === "All") return root.allStates[0]
        for (let i = 0; i < root.allStates.length; ++i) {
            if (root.allStates[i].algorithm === root.selectedAlgorithm) return root.allStates[i]
        }
        return root.allStates[0]
    }

    signal selectedAlgorithmChangedByUser(string algorithmName)
    signal allocateClicked()
    signal deallocateClicked()
    signal resetClicked()
    signal autoDemoClicked()

    ScrollView {
        anchors.fill: parent
        anchors.margins: 12
        clip: true

        Column {
            width: root.width - 24
            spacing: 10

            Label {
                text: "⬢ MemSim"
                color: Theme.textPrimary
                font.bold: true
                font.pixelSize: 20
            }

            Rectangle { width: parent.width; height: 1; color: Theme.deepSurface }

            Label { text: "ALGORITHMS"; color: Theme.textSecondary; font.bold: true }

            Repeater {
                model: ["First-Fit", "Next-Fit", "Best-Fit", "Worst-Fit", "All"]
                delegate: Button {
                    width: parent.width
                    text: modelData
                    highlighted: root.selectedAlgorithm === modelData
                    onClicked: root.selectedAlgorithmChangedByUser(modelData)
                }
            }

            Rectangle { width: parent.width; height: 1; color: Theme.deepSurface }
            Label { text: "ACTIONS"; color: Theme.textSecondary; font.bold: true }

            Button { width: parent.width; text: "+ Allocate"; onClicked: root.allocateClicked() }
            Button { width: parent.width; text: "− Deallocate"; onClicked: root.deallocateClicked() }
            Button { width: parent.width; text: "⟳ Reset All"; onClicked: root.resetClicked() }
            Button { width: parent.width; text: "▶ Auto Demo"; onClicked: root.autoDemoClicked() }

            Rectangle { width: parent.width; height: 1; color: Theme.deepSurface }

            Label { text: "POOL STATS"; color: Theme.textSecondary; font.bold: true }

            Label {
                color: Theme.textPrimary
                text: {
                    const state = root.selectedState()
                    const free = state ? state.totalFreeKB : 0
                    const algo = state ? state.algorithm : ""
                    return (algo ? algo + "\n" : "") + "Total 1024 KB\nFree " + free + " KB"
                }
            }

            Label { text: "Made by Ehtisham Haider"; color: Theme.textSecondary; font.pixelSize: 11 }
        }
    }
}
