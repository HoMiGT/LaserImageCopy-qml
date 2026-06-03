import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 792
    title: qsTr("镭射图片拷贝")

    property string errorMessages: ""

    Timer {
        id: errorDialogTimer
        interval: 50
        repeat: false
        onTriggered: {
            appErrorDialog.text = root.errorMessages.trim();
            appErrorDialog.open();
            root.errorMessages = "";
        }
    }

    Connections {
        target: backend
        function onPathNotExist(kind, path) {
            let prefix = (kind === "src") ? "原始路径" : "目标路径";
            root.errorMessages += prefix + "不存在: " + path + "\n";
            errorDialogTimer.restart();
        }
        function onCopyFinished(message) {
            root.errorMessages += message + "\n";
            errorDialogTimer.restart();
        }
    }

    Popup {
        id: appErrorDialog
        width: 420
        height: implicitHeight
        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property string text: ""

        background: Rectangle {
            color: "#FFFFFF"
            radius: 16
            border.color: "#E5E7EB"
            border.width: 1
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#40000000"
                shadowBlur: 32
                shadowVerticalOffset: 4
            }
        }

        contentItem: ColumnLayout {
            spacing: 24

            Text {
                text: "提示信息"
                font.pixelSize: 20
                font.weight: Font.Bold
                color: "#1F2937"
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 8
            }

            Text {
                text: appErrorDialog.text
                font.pixelSize: 16
                color: "#4B5563"
                wrapMode: Text.Wrap
                lineHeight: 1.4
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.maximumWidth: 360
                Layout.alignment: Qt.AlignHCenter
            }

            Button {
                text: "确定"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 140
                Layout.preferredHeight: 44
                Layout.bottomMargin: 8
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                background: Rectangle {
                    radius: 8
                    color: parent.down ? "#2563EB" : (parent.hovered ? "#60A5FA" : "#3B82F6")
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowColor: "#403B82F6"
                        shadowBlur: 12
                        shadowVerticalOffset: 2
                    }
                }

                onClicked: {
                    appErrorDialog.close()
                }
            }
        }
    }

    Screen01 {
        id: page
        anchors.fill: parent

        srcModel: backend.srcModel
        dstModel: backend.dstModel

        onChoice8k: function () {
            let path = backend.get8kPath();
            srcPathText = path.src;
            dstPathText = path.dst;
            page.srcPathSelector.performScan();
            page.dstPathSelector.performScan();
        }

        onChoice16k: function () {
            let path = backend.get16kPath();
            srcPathText = path.src;
            dstPathText = path.dst;
            page.srcPathSelector.performScan();
            page.dstPathSelector.performScan();
        }

        onChoiceCustom: function () {
            srcPathText = "";
            dstPathText = "";
            page.srcPathSelector.performScan();
            page.dstPathSelector.performScan();
        }
    }

    Component.onCompleted: {
        page.choice8k();
    }
}
