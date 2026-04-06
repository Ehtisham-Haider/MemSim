pragma Singleton
import QtQuick
import QtQuick.Controls
import "./"

QtObject {
    readonly property int spacing: 10
    readonly property int radius: 8
    readonly property int titleSize: 20
    readonly property int bodySize: 13

    function panelColor() {
        return Theme.panelSurface
    }

    function blockColor(isFree, processId) {
        if (isFree) {
            return Theme.freeBlock
        }

        const palette = [Theme.firstFit, Theme.nextFit, Theme.bestFit, Theme.worstFit, "#81C784", "#BA68C8", "#64B5F6", "#FFD54F"]
        return palette[Math.abs(processId) % palette.length]
    }

    function algorithmColor(name) {
        if (name === "First-Fit") return Theme.firstFit
        if (name === "Next-Fit") return Theme.nextFit
        if (name === "Best-Fit") return Theme.bestFit
        if (name === "Worst-Fit") return Theme.worstFit
        return Theme.textSecondary
    }
}
