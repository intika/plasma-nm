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

#ifndef PLASMA_NM_NETWORK_SETTINGS_H
#define PLASMA_NM_NETWORK_SETTINGS_H

#include <QObject>
#include "networkmodelitem.h"

class NetworkSettingsPrivate;

class NetworkSettings : public QObject
{
Q_OBJECT
Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
Q_PROPERTY(QObject* networkModel READ networkModel WRITE setNetworkModel NOTIFY networkModelChanged)

public:
    NetworkSettings();
    virtual ~NetworkSettings();

    QObject* networkModel();

    QString icon() const;
    QString name() const;
    QString status() const;

public Q_SLOTS:
    void setIcon(const QString & icon);
    void setName(const QString & name);
    void setStatus(const QString & status);
    void setNetworkModel(QObject* networkModel);
    void setNetwork(uint type, const QString & path);

private Q_SLOTS:
    void activeConnectionAdded(const QString & active);
    void updateIcon();
    void updateName();
    void updateStatus();

Q_SIGNALS:
    void iconChanged();
    void nameChanged();
    void statusChanged();
    void networkModelChanged();

private:
    NetworkSettingsPrivate* d;
};

#endif // PLASMA_NM_NETWORK_SETTINGS_H
