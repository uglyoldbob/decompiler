import QtQuick 2.0
import QtQuick.Controls 1.4

Column {
    property alias p: t1.text
    property alias model: tblv.model
    Text { id: t1; text: "blab" }
    TableView {
        id: tblv
        selectionMode: SelectionMode.SingleSelection
        TableViewColumn {
            title: "Name"
            role: "name"
            width: 50
        }
        TableViewColumn {
            title: "Col2"
            role: "fruitPrice"
            width: 50
        }
    }
    Button {
        text: "Analyze"
        anchors.right: parent.right
        onClicked: {
            console.log(tblv.model[tblv.currentRow].name)
        }
    }
}
