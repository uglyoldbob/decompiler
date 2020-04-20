import QtQuick 2.0
import QtQuick.Controls 1.4

Column {
    property alias p: t1.text
    property var initialModel
    Text { id: t1; text: "blab" }
    TableView {
        id: tblv
        selectionMode: SelectionMode.SingleSelection
        model: initialModel
        onDoubleClicked: {
            console.log("double click " + tblv.model[tblv.currentRow].name)
            if (tblv.model[tblv.currentRow].IsDir)
            {
                console.log("Is a directory")
                tblv.model = tblv.model[tblv.currentRow].DirContents
            }
            else
            {
                console.log("Is NOT a directory")
            }
        }
        TableViewColumn {
            title: "Name"
            role: "name"
            width: 50
            delegate: Row {
                Text { text: (tblv.model[styleData.row].IsDir) ? "D " : " " } //TODO: replace with images
                Text { text: styleData.value }
            }
        }
        TableViewColumn {
            title: "Col2"
            role: "size"
            width: 50
        }
    }
    Row {
        anchors.right: parent.right
        Button {
            text: "Reset"
            onClicked: {
                tblv.model = initialModel
            }
        }
        Button {
            text: "Analyze"
            onClicked: {
                if (tblv.currentRow != -1) {
                    console.log(tblv.model[tblv.currentRow].name)
                } else {
                    console.log("Nothing selected")
                }
            }
        }
    }
}
