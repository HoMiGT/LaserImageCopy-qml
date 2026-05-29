import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    id: root
    width: 1280
    height: 792
    title: qsTr("镭射图片拷贝")

    Screen01 {
        id: page
        anchors.fill: parent

        onChoice8k: function(){
            let path = backend.get8kPath()
            srcAbsPath = path.src;
            dstAbsPath = path.dst;
        }

        onChoice16k: function() {
            let path = backend.get16kPath()
            srcAbsPath = path.src;
            dstAbsPath = path.dst;
        }

        onChoiceCustom: function(){
            srcAbsPath = ""
            dstAbsPath = ""
        }
    }

    Component.onCompleted: {
        page.choice8k()
    }
}
