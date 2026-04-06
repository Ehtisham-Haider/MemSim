import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    property var report: ({})

    radius: 8
    color: Theme.panelSurface
    border.color: Theme.deepSurface

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 5
        rowSpacing: 6
        columnSpacing: 10

        Label { text: "Metric"; color: Theme.textPrimary; font.bold: true }
        Label { text: "First-Fit"; color: Theme.firstFit; font.bold: true }
        Label { text: "Next-Fit"; color: Theme.nextFit; font.bold: true }
        Label { text: "Best-Fit"; color: Theme.bestFit; font.bold: true }
        Label { text: "Worst-Fit"; color: Theme.worstFit; font.bold: true }

        Label { text: "Success Rate"; color: Theme.textSecondary }
        Label { text: (report.successRates || {})["First-Fit"] + "%"; color: Theme.textPrimary }
        Label { text: (report.successRates || {})["Next-Fit"] + "%"; color: Theme.textPrimary }
        Label { text: (report.successRates || {})["Best-Fit"] + "%"; color: Theme.textPrimary }
        Label { text: (report.successRates || {})["Worst-Fit"] + "%"; color: Theme.textPrimary }

        Label { text: "Largest Free Block"; color: Theme.textSecondary }
        Label { text: ((report.largestFreeBlocks || {})["First-Fit"] ?? 0) + " KB"; color: Theme.textPrimary }
        Label { text: ((report.largestFreeBlocks || {})["Next-Fit"] ?? 0) + " KB"; color: Theme.textPrimary }
        Label { text: ((report.largestFreeBlocks || {})["Best-Fit"] ?? 0) + " KB"; color: Theme.textPrimary }
        Label { text: ((report.largestFreeBlocks || {})["Worst-Fit"] ?? 0) + " KB"; color: Theme.textPrimary }
    }
}
