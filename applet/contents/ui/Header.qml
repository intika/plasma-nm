/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents

PlasmaComponents.ListItem {
    id: header;

    property alias text: headerLabel.text;

    anchors {
        left: parent.left;
        right: parent.right;
    }

    height: headerLabel.height + padding.margins.top + padding.margins.bottom;
    sectionDelegate: true;

    PlasmaComponents.Label {
        id: headerLabel;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
        }

        height: paintedHeight;
        font.weight: Font.DemiBold;
    }

    PlasmaComponents.ToolButton {
        id: refreshButton;

        anchors {
            bottom: parent.bottom;
            bottomMargin: -padding.margins.bottom + 2;
            right: parent.right;
            rightMargin: padding.margins.right;
            top: parent.top;
            topMargin: -padding.margins.top + 2;
        }

        iconSource: "view-refresh";
        visible: header.text == i18n("Available connections");

        onClicked: {
            handler.requestScan();
        }
    }
}