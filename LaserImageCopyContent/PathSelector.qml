import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LaserImageCopy 1.0
import QtQuick.Dialogs

Item {
    id: root

    property string labelText: qsTr("原始路径:")
    property string pathText: ""
    property string kind: "src"

    implicitWidth: 350
    implicitHeight: 50

    RowLayout{
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        spacing: 12

        Text {
            id: label
            text: root.labelText
            font.pixelSize: 14
            font.weight: Font.Medium
            color: "#374151"
            Layout.alignment: Qt.AlignVCenter
        }

        Item {
            id: inputGroup
            implicitHeight: 36
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            Rectangle {
                anchors.fill: parent
                color: "#F9FAFB"
                radius: 8
                border.color: "#D1D5DB"
                border.width: 1
            }

            TextInput {
                id: field
                text: root.pathText
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                verticalAlignment: TextInput.AlignVCenter
                font.pixelSize: 13
                color: "#1F2937"
                clip: true
                selectByMouse: true
            }
        }

        Button {
            id: button
            text: qsTr("浏览")
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: 36
            
            background: Rectangle {
                color: button.down ? "#E5E7EB" : (button.hovered ? "#F3F4F6" : "#FFFFFF")
                radius: 8
                border.color: "#D1D5DB"
                border.width: 1
            }
            contentItem: Text {
                text: button.text
                font.pixelSize: 13
                font.weight: Font.Medium
                color: "#374151"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                folderDialog.open()
            }
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
