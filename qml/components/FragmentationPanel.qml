import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Rectangle {
    id: root
    property var state: ({})

    radius: 8
    color: Theme.panelSurface
    border.color: Theme.deepSurface

    ScrollView {
        anchors.fill: parent
        anchors.margins: 10
        clip: true

        Column {
            width: parent.width
            spacing: 4

            Label { text: "Fragmentation Report"; color: Theme.textPrimary; font.bold: true }
            Label { text: "Total Free: " + (state.totalFreeKB || 0) + " KB"; color: Theme.textSecondary }
            Label { text: "Free Blocks: " + (state.freeBlockCount || 0); color: Theme.textSecondary }
            Label { text: "Largest: " + (state.largestFreeBlockKB || 0) + " KB"; color: Theme.textSecondary }
            Label { text: "External Fragmentation: " + ((state.externalFragmentation || false) ? "YES" : "NO"); color: Theme.textSecondary }
            Label { text: "Ratio: " + Number(state.fragmentationRatio || 0).toFixed(2); color: Theme.textSecondary }
        }
    }
}
