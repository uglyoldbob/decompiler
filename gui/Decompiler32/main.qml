import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.2
import uglyoldbob 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Decompiler for 32 bit programs")
    ListModel {
        id: speedModel
        Component.onCompleted: {
            [
                {
                    name: "Turtle",
                    speed: slowSpeed()
                },
                {
                    name: "Rabbit",
                    speed: highSpeed()
                }
            ].forEach(function(e) { append(e); });
        }
    }
    Column {
        Repeater {
            model: speedModel
            Text {
                text: model.name + " " + model.speed
            }
        }
        Text {
            text: SDecompileProject.bob[0] + " " + SDecompileProject.bob.length + " blength"
        }
        Text {
            text: SDecompileProject.Things[0] + " " + SDecompileProject.Things.length + " length"
        }

        Repeater {
            model: SDecompileProject.Things
            Text {
                text: model + " stuff"
            }
        }
        Repeater {
            model: SDecompileProject.bob
            Text {
                text: model.modelData + " is groot yay"
            }
        }

        Button {
            text: "Add"
            onClicked: {
                speedModel.append({name: "Bird", speed: 60});
            }
        }
    }
    function slowSpeed() {
        return 12;
    }
    function highSpeed() {
        return 42;
    }
}
