import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import uglyoldbob 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Decompiler for 32 bit programs")

    Column {
        Text {
            text: SDecompileProject.Things[0] + " " + SDecompileProject.Things.length + " length"
        }

        Repeater {
            model: SDecompileProject.Things
            Text {
                text: model + " stuff"
            }
        }

        ListView {
            id: list
            width: 180; height: 200
            model: SDecompileProject.bob
            delegate: Item {
                width: parent.width
                height: txt1.height
                Column {
                    Text
                    {
                        id: txt1
                        text: (index+1) + " - " + model.modelData
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

            }
        }
    }
}
