import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "./components"
import "./pages"
import "./dialogs"
import "./style"

ApplicationWindow {
    id: window
    visible: true
    width: Math.min(1360, Screen.width - 40)
    height: Math.min(860, Screen.height - 80)
    minimumWidth: 960
    minimumHeight: 620
    x: Math.max(0, (Screen.width - width) / 2)
    y: Math.max(0, (Screen.height - height) / 2)
    title: "MemSim v1.0 — Memory Allocation Simulator"
    color: Theme.appBackground

    property string selectedAlgorithm: "All"

    header: Rectangle {
        color: Theme.deepSurface
        height: 56
        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            Label {
                text: "MemSim — Dynamic Memory Allocation Simulator"
                color: Theme.textPrimary
                font.pixelSize: 18
                font.bold: true
            }
            Item { Layout.fillWidth: true }
            Label {
                text: simController.simulationRunning ? "Stress Test Running..." : "Ready"
                color: simController.simulationRunning ? Theme.nextFit : Theme.success
                font.pixelSize: 13
            }
        }
    }

    AllocateDialog {
        id: allocateDialog
        selectedAlgorithm: window.selectedAlgorithm
        onAllocateRequested: function(sizeKB, processId, processName, algorithmName) {
            simController.allocate(sizeKB, processId, processName, algorithmName)
        }
    }

    DeallocateDialog {
        id: deallocateDialog
        selectedAlgorithm: window.selectedAlgorithm
        allStates: simController.allAlgorithmStates
        onDeallocateRequested: function(blockId, algorithmName) {
            simController.deallocate(blockId, algorithmName)
        }
    }

    SplitView {
        anchors.fill: parent

        Sidebar {
            id: sidebar
            SplitView.minimumWidth: 200
            SplitView.maximumWidth: 220
            SplitView.preferredWidth: 210
            selectedAlgorithm: window.selectedAlgorithm
            allStates: simController.allAlgorithmStates

            onSelectedAlgorithmChangedByUser: function(algorithmName) { window.selectedAlgorithm = algorithmName }
            onAllocateClicked: allocateDialog.open()
            onDeallocateClicked: deallocateDialog.open()
            onResetClicked: simController.resetAll()
            onAutoDemoClicked: simController.loadPreset("Demo 1")
        }

        Rectangle {
            color: Theme.appBackground
            SplitView.fillWidth: true
            SplitView.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                TabBar {
                    id: tabBar
                    Layout.fillWidth: true
                    background: Rectangle { color: Theme.panelSurface; radius: 8 }
                    TabButton { text: "Simulation" }
                    TabButton { text: "All Algorithms" }
                    TabButton { text: "Stress Test" }
                    TabButton { text: "Compare" }
                    TabButton { text: "Log" }
                }

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: tabBar.currentIndex

                    SimulationPage {
                        selectedAlgorithm: window.selectedAlgorithm
                        allStates: simController.allAlgorithmStates
                        eventLog: simController.eventLog
                        onAllocateClicked: allocateDialog.open()
                        onDeallocateClicked: deallocateDialog.open()
                    }

                    AllAlgorithmsPage {
                        allStates: simController.allAlgorithmStates
                        onRunSameRequests: simController.loadPreset("Demo 1")
                        onResetAll: simController.resetAll()
                    }

                    StressTestPage {
                        running: simController.simulationRunning
                        progress: simController.stressProgress
                        resultsRows: simController.stressResults
                        summary: simController.stressSummary
                        onRunStressTest: function(useFixedSeed, fixedSeed) {
                            simController.runStressTest(useFixedSeed, fixedSeed)
                        }
                    }

                    ComparativePage {
                        comparativeReport: simController.comparativeReport
                    }

                    EventLogPage {
                        eventLog: simController.eventLog
                    }
                }
            }
        }
    }
}
