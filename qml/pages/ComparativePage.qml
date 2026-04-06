import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../style"

Item {
    id: root
    property var comparativeReport: ({})

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        ComparisonTable {
            Layout.fillWidth: true
            Layout.preferredHeight: 180
            report: root.comparativeReport
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 170
            radius: 8
            color: Theme.panelSurface
            border.color: Theme.deepSurface

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                Label { text: "Fragmentation Ratio"; color: Theme.textPrimary; font.bold: true }
                MetricBar {
                    label: "First-Fit"
                    value: (root.comparativeReport.fragmentationRatios || {})["First-Fit"] || 0
                    barColor: Theme.firstFit
                }
                MetricBar {
                    label: "Next-Fit"
                    value: (root.comparativeReport.fragmentationRatios || {})["Next-Fit"] || 0
                    barColor: Theme.nextFit
                }
                MetricBar {
                    label: "Best-Fit"
                    value: (root.comparativeReport.fragmentationRatios || {})["Best-Fit"] || 0
                    barColor: Theme.bestFit
                }
                MetricBar {
                    label: "Worst-Fit"
                    value: (root.comparativeReport.fragmentationRatios || {})["Worst-Fit"] || 0
                    barColor: Theme.worstFit
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: Theme.panelSurface
            border.color: Theme.deepSurface

            ScrollView {
                anchors.fill: parent
                anchors.margins: 10
                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: root.comparativeReport.justification || "Run simulation or stress test to generate justification."
                    color: Theme.textSecondary
                }
            }
        }
    }
}
