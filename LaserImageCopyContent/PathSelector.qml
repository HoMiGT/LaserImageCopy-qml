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

    signal flushPathText

    onPathTextChanged: {
        if (field.text !== pathText) {
            field.text = pathText;
        }
    }

    RowLayout {
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
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: field.activeFocus ? 2 : 1
                color: field.activeFocus ? "#3B82F6" : "#D1D5DB"

                // 平滑过渡
                Behavior on color {
                    ColorAnimation {
                        duration: 150
                    }
                }
                Behavior on height {
                    NumberAnimation {
                        duration: 150
                    }
                }
            }

            TextInput {
                id: field
                text: root.pathText
                anchors.fill: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                anchors.bottomMargin: 4
                verticalAlignment: TextInput.AlignVCenter
                font.pixelSize: 14
                color: "#1F2937"
                clip: true
                selectByMouse: true

                onAccepted: {
                    root.performScan();
                }
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
                folderDialog.open();
            }
        }
    }

    function performScan() {
        // 使用 typeof 避免在独立测试组件时报错
        let cameraType = "custom";
        if (typeof radioButton1 !== "undefined" && radioButton1.checked) {
            cameraType = "8k";
        } else if (typeof radioButton2 !== "undefined" && radioButton2.checked) {
            cameraType = "16k";
        }
        backend.scanDirectory(field.text.trim(), cameraType, root.kind);
    }

    function updatePath(urlValue) {
        root.pathText = backend.toLocalPath(urlValue);
        field.text = root.pathText;
        root.performScan();
    }

    FolderDialog {
        id: folderDialog
        title: "选择文件夹"

        onAccepted: {
            root.updatePath(selectedFolder);
        }
    }
}
