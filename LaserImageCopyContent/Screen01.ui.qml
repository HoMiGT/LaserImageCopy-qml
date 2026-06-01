

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import LaserImageCopy

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height

    color: "#F5F7FA" // 极其现代、舒适的灰蓝色背景
    layer.smooth: true

    property alias srcAbsPath: srcPath.pathText
    property alias dstAbsPath: dstPath.pathText
    
    property alias srcModel: srcPathListView.model
    property alias dstModel: dstPathListView.model

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
            Layout.preferredHeight: 70
            Layout.topMargin: 24
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            
            Rectangle {
                anchors.fill: parent
                color: "#FFFFFF"
                radius: 16
                border.color: "#E5E7EB"
                border.width: 1
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#10000000"
                    shadowBlur: 20
                }
            }

            RowLayout {
                id: radioButtonRow
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 24
                spacing: 30

                RadioButton {
                    id: radioButton1
                    text: qsTr("8K产线")
                    font.weight: Font.Medium
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
                    font.weight: Font.Medium
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
                    font.weight: Font.Medium
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
                    font.weight: Font.Medium
                    font.pointSize: 12
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
                spacing: 20
                
                Rectangle {
                    id: rectangle1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#FFFFFF"
                    radius: 16
                    border.color: "#E5E7EB"
                    border.width: 1
                    transformOrigin: Item.Center

                    PathSelector {
                        id: srcPath
                        labelText: "原始路径:"
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 20
                        anchors.leftMargin: 24
                        anchors.rightMargin: 24
                    }
                    
                    PathListView {
                        id: srcPathListView
                        anchors.top: srcPath.bottom
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 16
                        anchors.bottomMargin: 20
                        anchors.leftMargin: 24
                        anchors.rightMargin: 24
                    }
                }
                
                RoundButton {
                    id: roundButton
                    Layout.preferredWidth: 64
                    Layout.preferredHeight: 64
                    icon.height: 36
                    icon.width: 36
                    icon.source: "icons/move-to-folder.svg"
                    
                    // 使其看起来更活泼、现代
                    background: Rectangle {
                        radius: 32
                        color: roundButton.down ? "#2563EB" : (roundButton.hovered ? "#60A5FA" : "#3B82F6")
                        layer.enabled: true
                        layer.effect: MultiEffect {
                            shadowEnabled: true
                            shadowColor: "#403B82F6"
                            shadowBlur: 16
                        }
                    }
                    icon.color: "#FFFFFF"
                }
                
                Rectangle {
                    id: rectangle2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#FFFFFF"
                    radius: 16
                    border.color: "#E5E7EB"
                    border.width: 1

                    PathSelector {
                        id: dstPath
                        labelText: "目标路径:"
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 20
                        anchors.leftMargin: 24
                        anchors.rightMargin: 24
                    }
                    
                    PathListView {
                        id: dstPathListView
                        anchors.top: dstPath.bottom
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 16
                        anchors.bottomMargin: 20
                        anchors.leftMargin: 24
                        anchors.rightMargin: 24
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
