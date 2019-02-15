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
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.4

ComboBox {
    id: combobox
    property int fontSize: 12
    property bool fontBold: true

    style: ComboBoxStyle {
        id: cbStyle
        dropDownButtonWidth: 40
        __dropDownStyle: MenuStyle {
            font: cbStyle.font
            __labelColor: cbStyle.textColor
            __selectedLabelColor: cbStyle.selectedTextColor
            __selectedBackgroundColor: cbStyle.selectionColor
            __backgroundColor: "#f5f5f5"
            __maxPopupHeight: 400
            __menuItemType: "comboboxitem"
            scrollIndicator: Image{}
            __scrollerStyle: ScrollViewStyle {
                transientScrollBars: false
            }
        }
        Component.onCompleted: {
            cbStyle.font.pointSize = combobox.fontSize
            cbStyle.font.bold = combobox.fontBold
        }

        background: Item {
//            implicitWidth: Math.round(TextSingleton.implicitHeight * 4.5)
//            implicitHeight: Math.max(25, Math.round(TextSingleton.implicitHeight * 1.2))
            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: control.pressed ? 0 : -1
                color: "#f5f5f5"
                radius: baserect.radius
            }
            Rectangle {
                id: baserect
                /*gradient: Gradient {
                    GradientStop {color: control.pressed ? "#bababa" : "#fefefe" ; position: 0}
                    GradientStop {color: control.pressed ? "#ccc" : "#e3e3e3" ; position: 1}
                }*/
//              radius: TextSingleton.implicitHeight * 0.16
                anchors.fill: parent
                color: "#f5f5f5"
                border.color: "#c8c8c8"
                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    color: control.activeFocus ? "#47b" : "white"
                    opacity: control.hovered || control.activeFocus ? 0.1 : 0
                    Behavior on opacity {NumberAnimation{ duration: 100 }}
                }
            }
            Image {
                id: imageItem
                visible: control.menu !== null
                source: "../images/btn_down.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: dropDownButtonWidth / 2
                opacity: control.enabled ? 0.6 : 0.3
            }
        }
    }
}
