

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LaserImageCopy

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height

    color: Constants.backgroundColor
    layer.smooth: true

    property alias srcAbsPath: srcPath.pathText
    property alias dstAbsPath: dstPath.pathText

    property bool isVisible: false

    signal choice8k()
    signal choice16k()
    signal choiceCustom()

    ColumnLayout {
        id: rectangleColumn
        anchors.fill: parent

        Item {
            id: radioButtonGroup
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            Layout.topMargin: 20
            Layout.leftMargin: 30
            Layout.rightMargin: 30

            RowLayout {
                id: radioButtonRow
                anchors.fill: parent
                spacing: 30

                RadioButton {
                    id: radioButton1
                    text: qsTr("8K产线")
                    font.bold: true
                    font.pointSize: 12
                    checked: true
                    onCheckedChanged: {
                        if (checked){
                            rectangle.choice8k()
                        }
                    }
                }

                RadioButton {
                    id: radioButton2
                    text: qsTr("16K产线")
                    font.bold: true
                    font.pointSize: 12
                    onCheckedChanged: {
                        if (checked){
                            isVisible = true
                            rectangle.choice16k()
                        }else{
                            isVisible = false
                        }
                    }
                }

                RadioButton {
                    id: radioButton3
                    text: qsTr("自定义")
                    font.bold: true
                    font.pointSize: 12
                    onCheckedChanged: {
                        if(checked){
                             rectangle.choiceCustom()
                        }
                    }
                }

                CheckBox{
                    id: isSplitImage
                    text: "是否切分图片"
                    visible: isVisible
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            id: rectangleGroup
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            Layout.bottomMargin: 30

            RowLayout {
                id: rectnagleGroupRow
                anchors.fill: parent
                spacing: 10
                Rectangle {
                    id: rectangle1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    radius: 50
                    transformOrigin: Item.Center
                    PathSelector {
                        id: srcPath
                        labelText: "原始路径:"
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 10
                        anchors.leftMargin: 30
                    }
                }
                RoundButton {
                    id: roundButton
                    Layout.preferredWidth: 129
                    Layout.preferredHeight: 129
                    icon.height: 80
                    icon.width: 80
                    icon.source: "icons/move-to-folder.svg"
                }
                Rectangle {
                    id: rectangle2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 50
                    color: "#ffffff"

                    PathSelector {
                        id: dstPath
                        labelText: "目标路径:"
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 10
                        anchors.leftMargin: 30
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "clicked"
        }
    ]
}
