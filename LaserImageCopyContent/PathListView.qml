import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Item {
    id: root

    // 允许外部传入 model
    property alias model: listView.model

    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10
        clip: true

        delegate: Item {
            width: ListView.view.width
            height: 76 // 增加了一点高度来容纳悬浮时的放大和边距

            Rectangle {
                id: delegateItem
                // 留出一定的边距 (margin)，这样放大时就不会超过父级 Item 而被裁切
                anchors.fill: parent
                anchors.topMargin: 4
                anchors.bottomMargin: 4
                anchors.leftMargin: 8
                anchors.rightMargin: 8

                // 悬浮时背景变浅灰，平时纯白
                color: hoverArea.containsMouse ? "#F9FAFB" : "#FFFFFF"
                radius: 12
                // 悬浮时边框加深
                border.color: hoverArea.containsMouse ? "#D1D5DB" : "#F3F4F6"
                border.width: 1

                // 增加一点悬浮放大效果，让交互更明显
                scale: hoverArea.containsMouse ? 1.02 : 1.0

                // 平滑动画过渡
                Behavior on color {
                    ColorAnimation {
                        duration: 150
                    }
                }
                Behavior on border.color {
                    ColorAnimation {
                        duration: 150
                    }
                }
                Behavior on scale {
                    NumberAnimation {
                        duration: 150
                        easing.type: Easing.OutQuad
                    }
                }

                MouseArea {
                    id: hoverArea
                    anchors.fill: parent
                    hoverEnabled: true
                    // 鼠标移上去变成小手
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        detailPopup.folderName = model.folderName !== undefined ? model.folderName : "未知";
                        detailPopup.sumCount = model.sumCount !== undefined ? model.sumCount : 0;
                        detailPopup.currentCount = model.currentCount !== undefined ? model.currentCount : 0;
                        detailPopup.open();
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 20

                    // 名字显示
                    Text {
                        text: model.folderName !== undefined ? model.folderName : "未知名称"
                        font.pixelSize: 15
                        font.weight: Font.Medium
                        color: "#1F2937"
                        Layout.preferredWidth: 130
                        elide: Text.ElideRight
                    }

                    // 进度条显示 (自定义现代化样式)
                    ProgressBar {
                        id: progressBar
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        value: (model.sumCount !== undefined && model.sumCount > 0) ? (model.currentCount / model.sumCount) : 0

                        background: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 8
                            color: "#E5E7EB"
                            radius: 4
                        }
                        contentItem: Item {
                            implicitWidth: 200
                            implicitHeight: 8
                            Rectangle {
                                width: progressBar.visualPosition * parent.width
                                height: parent.height
                                radius: 4
                                color: "#3B82F6" // 现代蓝
                            }
                        }
                    }

                    // 数据显示
                    Text {
                        text: (model.currentCount !== undefined ? model.currentCount : "0") + " / " + (model.sumCount !== undefined ? model.sumCount : "0")
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: "#6B7280"
                        Layout.preferredWidth: 80
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
        } // delegate结束

        // 当列表为空时显示提示语 (从delegate移出)
        Text {
            anchors.centerIn: parent
            text: qsTr("暂无数据")
            color: "#999999"
            font.pixelSize: 14
            visible: listView.count === 0
        }
    }

    // 全局居中显示的详情弹窗
    Popup {
        id: detailPopup
        parent: Overlay.overlay
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)
        width: 320
        height: 220
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property string folderName: ""
        property int sumCount: 0
        property int currentCount: 0

        background: Rectangle {
            color: "#FFFFFF"
            radius: 16
            border.color: "#E5E7EB"
            border.width: 1
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 20
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            Text {
                text: "详细信息"
                font.pixelSize: 16
                font.weight: Font.Bold
                color: "#1F2937"
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#F3F4F6"
            }

            Text {
                text: "路径全称: " + detailPopup.folderName
                font.pixelSize: 15
                color: "#4B5563"
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }

            Text {
                text: "当前进度: " + detailPopup.currentCount + " / " + detailPopup.sumCount
                font.pixelSize: 15
                color: "#4B5563"
                Layout.fillWidth: true
            }

            Item {
                Layout.fillHeight: true
            }

            Button {
                id: closeBtn
                text: "确定"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 100
                Layout.preferredHeight: 36
                background: Rectangle {
                    color: closeBtn.down ? "#2563EB" : (closeBtn.hovered ? "#60A5FA" : "#3B82F6")
                    radius: 8
                }
                contentItem: Text {
                    text: closeBtn.text
                    color: "#FFFFFF"
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: detailPopup.close()
            }
        }
    }
}
