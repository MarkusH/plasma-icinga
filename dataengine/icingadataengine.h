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

#ifndef ICINGADATAENGINE_H
#define ICINGADATAENGINE_H

#include <QNetworkAccessManager>

#include <KConfig>
#include <KConfigGroup>

#include <Plasma/DataEngine>

#include <qjson/parser.h>

class QDateTime;

class IcingaDataEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    IcingaDataEngine(QObject *parent, const QVariantList &args);

protected:
    bool sourceRequestEvent(const QString &name);
    bool updateSourceEvent(const QString &source);

private:
    Data interpretData(const QMap< QString, QVariant > result);
    int stateToInt(const QString state);

    QNetworkAccessManager m_manager;
    QJson::Parser m_parser;
    QString m_name;
    QUrl m_url;
    KConfig m_config;
    KConfigGroup m_generalcg;
    QDateTime m_lastUpdate;
    bool m_hasNetwork;

private Q_SLOTS:
    void updateData();
    void networkDown();
    void networkUp();
};

#endif // ICINGADATAENGINE_H
