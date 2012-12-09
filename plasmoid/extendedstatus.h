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

#ifndef EXTENDEDSTATUS_H
#define EXTENDEDSTATUS_H

#include <QtGui/QGraphicsGridLayout>
#include <QtGui/QGraphicsLayoutItem>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsWidget>

#include <Plasma/Label>
#include <Plasma/ScrollWidget>

class QDateTime;

class ExtendedStatus : public QGraphicsWidget
{

public:
    ExtendedStatus(QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~ExtendedStatus();
    void init();

    void addService(QGraphicsLayoutItem* item);
    void clearServices();
    void setStats(const QDateTime& lastChange, const int hosts, const int services, const int ok, const int warning, const int critical, const int unknown);

public Q_SLOTS:
    void updateSize();

    QGraphicsLinearLayout *m_layout;
    QGraphicsGridLayout *m_statsLayout;
    QGraphicsLinearLayout *m_logLayout;
    QGraphicsLinearLayout *m_logListLayout;

    Plasma::Label *m_lastUpdateLbl;
    Plasma::Label *m_lastUpdate;
    Plasma::Label *m_hostsLbl;
    Plasma::Label *m_hosts;
    Plasma::Label *m_servicesLbl;
    Plasma::Label *m_services;
    Plasma::Label *m_okLbl;
    Plasma::Label *m_ok;
    Plasma::Label *m_warningLbl;
    Plasma::Label *m_warning;
    Plasma::Label *m_criticalLbl;
    Plasma::Label *m_critical;
    Plasma::Label *m_unknownLbl;
    Plasma::Label *m_unknown;

    Plasma::Label *m_logLbl;
    QGraphicsWidget *m_logWg;
    Plasma::ScrollWidget *m_logScroll;

};

#endif // EXTENDEDSTATUS_H
