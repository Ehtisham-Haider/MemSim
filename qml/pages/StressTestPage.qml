import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Item {
    id: root
    property bool running: false
    property int progress: 0
    property var resultsRows: []
    property var summary: ({})

    signal runStressTest(bool useFixedSeed, int fixedSeed)

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            height: 58
            radius: 8
            color: Theme.panelSurface

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                Label { text: "STRESS TEST CONFIGURATION"; color: Theme.textPrimary; font.bold: true }
                Item { Layout.fillWidth: true }
                CheckBox { id: fixedSeedCheck; text: "Fixed Seed" }
                SpinBox { id: seedBox; from: 1; to: 9999; value: 42; enabled: fixedSeedCheck.checked }
                Button {
                    text: root.running ? "Running..." : "▶ Run Stress Test"
                    enabled: !root.running
                    onClicked: root.runStressTest(fixedSeedCheck.checked, seedBox.value)
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 54
            radius: 8
            color: Theme.panelSurface
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                ProgressBar {
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    value: root.progress
                }
                Label { text: root.progress + "%"; color: Theme.textPrimary }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: Theme.panelSurface
            border.color: Theme.deepSurface

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8

                Label { text: "RESULTS TABLE"; color: Theme.textPrimary; font.bold: true }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: root.resultsRows
                    clip: true

                    delegate: RowLayout {
                        width: ListView.view.width
                        spacing: 8

                        Label { text: "#" + modelData.request; color: Theme.textPrimary; Layout.preferredWidth: 40 }
                        Label { text: modelData.sizeKB + " KB"; color: Theme.textSecondary; Layout.preferredWidth: 80 }
                        Label { text: modelData["First-Fit"]; color: modelData["First-Fit"].startsWith("✓") ? Theme.success : Theme.failure; Layout.preferredWidth: 160 }
                        Label { text: modelData["Next-Fit"]; color: modelData["Next-Fit"].startsWith("✓") ? Theme.success : Theme.failure; Layout.preferredWidth: 160 }
                        Label { text: modelData["Best-Fit"]; color: modelData["Best-Fit"].startsWith("✓") ? Theme.success : Theme.failure; Layout.preferredWidth: 160 }
                        Label { text: modelData["Worst-Fit"]; color: modelData["Worst-Fit"].startsWith("✓") ? Theme.success : Theme.failure; Layout.fillWidth: true }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 64
            radius: 8
            color: Theme.panelSurface

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                Label { text: "First-Fit: " + (summary["First-Fit"] || 0) + "%"; color: Theme.firstFit }
                Label { text: "Next-Fit: " + (summary["Next-Fit"] || 0) + "%"; color: Theme.nextFit }
                Label { text: "Best-Fit: " + (summary["Best-Fit"] || 0) + "%"; color: Theme.bestFit }
                Label { text: "Worst-Fit: " + (summary["Worst-Fit"] || 0) + "%"; color: Theme.worstFit }
            }
        }
    }
}
