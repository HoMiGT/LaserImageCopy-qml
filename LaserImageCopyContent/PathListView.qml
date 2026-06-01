import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // 允许外部传入 model
    property alias model: listView.model

    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        clip: true

        delegate: Rectangle {
            width: ListView.view.width
            height: 68
            color: "#FFFFFF"
            radius: 12
            border.color: "#F3F4F6"
            border.width: 1

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
        
        // 当列表为空时显示提示语
        Text {
            anchors.centerIn: parent
            text: qsTr("暂无数据")
            color: "#999999"
            font.pixelSize: 14
            visible: listView.count === 0
        }
    }
}
