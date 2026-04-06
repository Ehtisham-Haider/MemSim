import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    modal: true
    title: "Deallocate Memory"
    standardButtons: Dialog.NoButton

    property string selectedAlgorithm: "All"
    property var allStates: []

    signal deallocateRequested(int blockId, string algorithmName)

    function allocatedBlocks() {
        let blocks = []
        for (let i = 0; i < root.allStates.length; ++i) {
            const state = root.allStates[i]
            if (root.selectedAlgorithm !== "All" && state.algorithm !== root.selectedAlgorithm) continue
            for (let j = 0; j < state.blocks.length; ++j) {
                const b = state.blocks[j]
                if (!b.isFree) {
                    blocks.push({
                        label: state.algorithm + " | BLK-" + b.blockId + " (" + b.processLabel + ", " + b.sizeKB + " KB)",
                        blockId: b.blockId,
                        algorithm: state.algorithm
                    })
                }
            }
        }
        return blocks
    }

    contentItem: ColumnLayout {
        spacing: 8

        ComboBox {
            id: blockBox
            model: root.allocatedBlocks()
            textRole: "label"
        }

        ComboBox {
            id: algoBox
            model: ["All", "First-Fit", "Next-Fit", "Best-Fit", "Worst-Fit"]
            currentIndex: Math.max(0, model.indexOf(root.selectedAlgorithm))
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            Button { text: "Cancel"; onClicked: root.close() }
            Button {
                text: "Deallocate"
                enabled: blockBox.currentIndex >= 0
                onClicked: {
                    const item = blockBox.model[blockBox.currentIndex]
                    if (!item) {
                        root.close()
                        return
                    }
                    root.deallocateRequested(item.blockId, algoBox.currentText)
                    root.close()
                }
            }
        }
    }
}
