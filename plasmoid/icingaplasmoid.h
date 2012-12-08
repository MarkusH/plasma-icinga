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

#ifndef ICINGAMONITOR_H
#define ICINGAMONITOR_H

#include <KConfig>
#include <KConfigGroup>

#include <Plasma/Applet>
#include <Plasma/DataEngine>

class QSizeF;

class IcingaPlasmoid : public Plasma::Applet
{
    Q_OBJECT

public:
    IcingaPlasmoid(QObject *parent, const QVariantList &args);
    void init();
    void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

protected slots:
    void constraintsEvent(Plasma::Constraints constraints);
    void dataUpdated( const QString& source, const Plasma::DataEngine::Data& data);
    void resetSize();

private:
    void updateSize();
    void expandFontToMax();

    int m_statusOk;
    int m_statusWarning;
    int m_statusCritical;
    int m_statusUnknown;
    int m_realstatus;
    int m_servicesCount;
    int m_hostsCount;
    KConfig m_config;
    KConfigGroup m_generalcg;
    QString m_text;
    QFont m_font;
};

#endif // ICINGAMONITOR_H
