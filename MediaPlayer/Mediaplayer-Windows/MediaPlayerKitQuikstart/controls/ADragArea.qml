import QtQuick 2.5

MouseArea {
    property var window: undefined
    anchors.fill: parent
    property point clickPos: "1,1"
    onPressed: {
        clickPos = Qt.point(mouse.x,mouse.y)
    }
    onPositionChanged: {
        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
        window.pos = Qt.point(window.pos.x+delta.x,
        window.pos.y+delta.y)
        //console.log(mouse.x-clickPos.x,mouse.y-clickPos.y)
    }
}
