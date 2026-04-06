import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../style"

Dialog {
    id: root
    modal: true
    title: "Allocate Memory"
    standardButtons: Dialog.NoButton

    property string selectedAlgorithm: "All"

    signal allocateRequested(int sizeKB, int processId, string processName, string algorithmName)

    contentItem: ColumnLayout {
        spacing: 8

        SpinBox { id: processIdSpin; from: 1; to: 99; value: 1; editable: true }
        TextField { id: processNameField; text: "P" + processIdSpin.value; placeholderText: "Process Name" }
        SpinBox { id: sizeSpin; from: 1; to: 500; value: 50; editable: true }

        ComboBox {
            id: algoBox
            model: ["All", "First-Fit", "Next-Fit", "Best-Fit", "Worst-Fit"]
            currentIndex: Math.max(0, model.indexOf(root.selectedAlgorithm))
        }

        Label {
            text: sizeSpin.value > 200 ? "⚠ Warning: this request may fail on some algorithms" : ""
            color: Theme.nextFit
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            Button { text: "Cancel"; onClicked: root.close() }
            Button {
                text: "Allocate"
                onClicked: {
                    root.allocateRequested(sizeSpin.value, processIdSpin.value, processNameField.text, algoBox.currentText)
                    root.close()
                }
            }
        }
    }
}
