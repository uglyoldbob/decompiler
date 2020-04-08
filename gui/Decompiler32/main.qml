import QtQuick 2.14
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.12
import QtQuick.Window 2.14
import uglyoldbob 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Decompiler for 32 bit programs")

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrl)
            DecompileProject.add_object(fileDialog.fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    Column {
        Text {
            text: DecompileProject.Objects[0] + " " + DecompileProject.Objects.length + " length"
        }

        ListView {
            id: list
            width: 180; height: 200
            model: DecompileProject.Objects
            delegate: Item {
                width: parent.width
                height: txt1.height
                Column {
                    Text
                    {
                        id: txt1
                        text: (index+1) + " - " + model.modelData.name
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: list.currentIndex = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        }

        Button {
            text: "Add"
            onClicked: {
                fileDialog.open()
            }
        }
    }
}
