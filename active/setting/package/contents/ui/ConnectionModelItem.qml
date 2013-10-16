/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.qtextracomponents 0.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.networkmanagement 0.1 as PlasmaNm

Item {
    property alias checked: connectionItem.checked;

    signal itemSelected();

    height: 48;
    anchors {
        left: parent.left;
        right: parent.right;
    }

    Rectangle {
        id: connectedItemBackground;

        height: 48;
        anchors {
            left: parent.left;
            right: parent.right;
        }
        color: "green";
        opacity: itemConnectionState == PlasmaNm.Enums.Activated ? 0.2 : 0.1;
        visible: itemConnectionState == PlasmaNm.Enums.Activated || itemConnectionState == PlasmaNm.Enums.Activating;
    }

    PlasmaComponents.ListItem {
        id: connectionItem;

        height: 48;
        anchors {
            left: parent.left;
            right: parent.right;
        }
        enabled: true

        PlasmaCore.Svg {
            id: svgIcons;

            multipleImages: true;
            imagePath: "icons/plasma-networkmanagement";
        }

        Item {
            id: connectionIcon;

            anchors {
                left: parent.left;
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 32;
            height: 48;

            PlasmaCore.SvgItem {
                id: svgConnectionIcon;

                anchors.fill: parent;
                svg: svgIcons;
                elementId: itemConnectionIcon;
                visible: itemType != PlasmaNm.Enums.Vpn && itemType != PlasmaNm.Enums.Adsl && itemType != PlasmaNm.Enums.Pppoe;
            }

            QIconItem {
                id: pngConnectionIcon;

                anchors.fill: parent;
                icon: QIcon("secure-card");
                visible: !svgConnectionIcon.visible;
            }
        }

        PlasmaComponents.Label {
            id: networkLabel;

            anchors {
                left: connectionIcon.right;
                right: securityLabel.left;
                verticalCenter: parent.verticalCenter;
                leftMargin: 10;
            }
            font.weight: Font.Bold;
            elide: Text.ElideRight;
            text: itemName;
        }

        QIconItem {
            id: securedIcon;

            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 48;
            height: 48;
            icon: QIcon("object-locked");
            visible: itemSecurityType != PlasmaNm.Enums.None;
        }

        PlasmaComponents.Label {
            id: securityLabel;

            anchors {
                right: securedIcon.left;
                top: parent.top;
                bottom: parent.bottom;
            }
            text: itemSecurityTypeString;
            visible: securedIcon.visible;
        }

        onClicked: itemSelected();
    }
}
