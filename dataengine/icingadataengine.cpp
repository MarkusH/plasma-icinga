/*
    Icinga Data Engine continuously retieves data from an Icinga instance
    Copyright (C) 2012  Markus Holtermann <info@markusholtermann.eu>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "icingadataengine.h"

#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <Plasma/DataContainer>

#include <Solid/Networking>

IcingaDataEngine::IcingaDataEngine(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args),
      m_manager(parent),
      m_parser(),
      m_name(),
      m_config("icingarc"),
      m_lastUpdate()
{
    qDebug() << "constructor";
    // We ignore any arguments - data engines do not have much use for them
    Q_UNUSED(args)

    m_generalcg = m_config.group("General");
    m_url = m_generalcg.readEntry("url", QString());
    m_hasNetwork = (Solid::Networking::status() == Solid::Networking::Connected);

    setMinimumPollingInterval(60000);
    connect(Solid::Networking::notifier(), SIGNAL(shouldDisconnect()), this, SLOT(networkDown()));
    connect(Solid::Networking::notifier(), SIGNAL(shouldConnect()), this, SLOT(networkUp()));
}

bool IcingaDataEngine::sourceRequestEvent(const QString &name)
{
    qDebug() << "sourceRequestEvent" << name;
    return updateSourceEvent(name);
}

bool IcingaDataEngine::updateSourceEvent(const QString &name)
{
    qDebug() << "updateSourceEvent" << name;
    m_name = name;
    setData(name, "connected", m_hasNetwork);
    if (m_hasNetwork) {
        setData(name, "error-msg", "");
        QNetworkRequest req(m_url);
        QNetworkReply *reply = m_manager.get(req);
        connect(reply, SIGNAL(finished()), this, SLOT(updateData()));
        //setData(name, DataEngine::Data());
    } else {
        setData(m_name, "error-msg", "Not connected to a network");
    }
    return true;
}

void IcingaDataEngine::updateData()
{
    qDebug() << "updateData";
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "updateData: no networ error";
        QByteArray data = reply->readAll();
        bool ok;
        QVariantMap result = m_parser.parse(data, &ok).toMap();
        m_lastUpdate = QDateTime::currentDateTime();
        if (!ok) {
            qDebug() << "Error parsing the json data";
            setData(m_name, "error-msg", reply->errorString());
        } else {
            setData(m_name, "error-msg", "");
            setData(m_name, interpretData(result));
        }
    } else {
        qDebug() << "updateData: network error";
        setData(m_name, "error-msg", reply->errorString());
    }
    reply->deleteLater();
}

Plasma::DataEngine::Data IcingaDataEngine::interpretData(const QMap<QString, QVariant> result)
{
    qDebug() << "interpreteData";
    Plasma::DataEngine::Data data;
    data["version"] = result["cgi_json_version"];
    data["last-update"] = m_lastUpdate;

    QVariantMap status = result["status"].toMap();
    QVariantList service_status = status["service_status"].toList();

    data["services-count"] = service_status.length();

    int stateType;
    int state[] = {0, 0, 0, 0, 0};
    QVariantMap service;
    QVariantMap hosts;
    QVariantList messages[] = {QVariantList(), QVariantList(), QVariantList(), QVariantList()};
    foreach (QVariant _service, service_status) {
        service = _service.toMap();
        stateType = stateToInt(service["status"].toString());
        state[stateType]++;
        QVariantMap msg;
        msg["host"] = service["host_display_name"];
        msg["service"] = service["service_display_name"];
        msg["downtime"] = service["in_scheduled_downtime"]; // bool
        msg["ack"] = service["has_been_acknowledged"]; // bool
        msg["msg"] = service["status_information"];
        messages[stateType] << msg;
        if (stateType > state[4] && !(service["in_scheduled_downtime"].toBool() || service["has_been_acknowledged"].toBool())) {
            state[4] = stateType;
        }
        if (!hosts.contains(service["host_name"].toString())) {
            hosts[service["host_name"].toString()] = service["host_display_name"].toString();
        }
    }
    data["hosts-count"] = hosts.count();
    data["hosts"] = hosts;

    data["status-ok"] = state[0];
    data["status-warning"] = state[1];
    data["status-critical"] = state[2];
    data["status-unknown"] = state[3];
    data["realstatus"] = state[4];

    data["msg-ok"] = messages[0];
    data["msg-warning"] = messages[1];
    data["msg-critical"] = messages[2];
    data["msg-unknown"] = messages[3];
    return data;
}

void IcingaDataEngine::networkDown()
{
    qDebug() << "networkDown";
    m_hasNetwork = false;
}

void IcingaDataEngine::networkUp()
{
    qDebug() << "networkUp";
    m_hasNetwork = true;
}

int IcingaDataEngine::stateToInt(const QString state)
{
    if (state == "OK") {
        return 0;
    } else if (state == "WARNING") {
        return 1;
    } else if (state == "CRITICAL") {
        return 2;
    }
    return 3;
}

K_EXPORT_PLASMA_DATAENGINE(icinga_dataengine, IcingaDataEngine)

#include "icingadataengine.moc"
