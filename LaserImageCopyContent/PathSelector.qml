import QtQuick
import QtQuick.Controls
import LaserImageCopy 1.0
import QtQuick.Dialogs

Item {
    id: root

    property string labelText: qsTr("原始路径:")
    property string pathText: ""
    property string kind: "src"

    height: 50
    width: 300

    Text {
        id: label
        text: "原始路径:"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12
    }

    Item {
        id: inputGroup
        anchors.left: label.right
        anchors.leftMargin: 12
        anchors.right: button.left
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        height: 30

        TextInput {
            id: field
            text: root.pathText
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: underline.top
            anchors.bottomMargin: 3
            height: 20
            font.pixelSize: 12
            clip: true
        }

        Rectangle {
            id: underline
            height: 1
            color: "#D3D3D3"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    RoundButton{
        id: button
        text: qsTr("...")
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        radius: 12

        onClicked: {
            folderDialog.open()
        }
    }

    function updatePath(urlValue){
        root.pathText = backend.toLocalPath(urlValue)
        field.text = root.pathText
    }

    FolderDialog{
        id: folderDialog
        title: "选择文件夹"

        onAccepted: {
            root.updatePath(selectedFolder)
        }
    }
}
