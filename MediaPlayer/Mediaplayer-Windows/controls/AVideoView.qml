/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.5
import AVideoWidget 1.0

Rectangle {
    id: video
    signal doubleClicked
    color: "#444444"
    property url backgroundImage: null
    property bool showVideo: false
    property alias videoWidget: videoWidget
    property double uid: 0
    MouseArea {
        id: ma
        anchors.fill: parent
        onDoubleClicked: {
            video.doubleClicked()
        }
    }
    AVideoWidget {
        id: videoWidget
        fillColor: parent.color
        visible: parent.showVideo
        anchors.fill: parent
    }
    Image {
        id: image
        visible: !parent.showVideo
        source: parent.backgroundImage
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        x: (parent.width-sourceSize.width)/2
        y: (parent.height-sourceSize.height)/2
        width: sourceSize.width
        height: sourceSize.height
    }
    function swap(rhs) {
        var x = rhs.x
        var y = rhs.y
        var z = rhs.z
        var width = rhs.width
        var height = rhs.height
        var anchors_top =  rhs.anchors.top
        var anchors_topMargin = rhs.anchors.topMargin
        var anchors_right = rhs.anchors.right
        var anchors_rightMargin = rhs.anchors.rightMargin

        rhs.x = video.x
        rhs.y = video.y
        rhs.z = video.z
        rhs.width = video.width
        rhs.height = video.height
        rhs.anchors.top = video.anchors.top
        rhs.anchors.topMargin = video.anchors.topMargin
        rhs.anchors.right = video.anchors.right
        rhs.anchors.rightMargin = video.anchors.rightMargin

        video.x = x
        video.y = y
        video.z = z
        video.width = width
        video.height = height
        video.anchors.top = anchors_top
        video.anchors.topMargin = anchors_topMargin
        video.anchors.right = anchors_right
        video.anchors.rightMargin = anchors_rightMargin
    }
}
