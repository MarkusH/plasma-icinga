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

#include "extendedstatus.h"

#include <QtCore/QDateTime>

#include <KGlobal>
#include <KLocale>

ExtendedStatus::ExtendedStatus(QGraphicsItem* parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(parent, wFlags)
{
    qDebug() << "ExtendedStatus constructor";
    setMinimumSize(5, 5);
    init();
}

ExtendedStatus::~ExtendedStatus()
{
    qDebug() << "ExtendedStatus destructor";
}

void ExtendedStatus::init()
{
    qDebug() << "ExtendedStatus init";
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(500, 160));
    setMaximumSize(QSize(700, 400));

    m_layout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_statsLayout = new QGraphicsGridLayout();
    m_statsLayout->setColumnAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
    m_logLayout = new QGraphicsLinearLayout(Qt::Vertical);

    m_lastUpdateLbl = new Plasma::Label(this);
    m_lastUpdateLbl->setText("Last Update:");
    m_statsLayout->addItem(m_lastUpdateLbl, 0, 0);

    m_lastUpdate = new Plasma::Label(this);
    m_lastUpdate->setText("");
    m_lastUpdate->setWordWrap(false);
    m_statsLayout->addItem(m_lastUpdate, 0, 1);

    m_hostsLbl = new Plasma::Label(this);
    m_hostsLbl->setText("Hosts:");
    m_statsLayout->addItem(m_hostsLbl, 1, 0);

    m_hosts = new Plasma::Label(this);
    m_hosts->setText("");
    m_statsLayout->addItem(m_hosts, 1, 1);

    m_servicesLbl = new Plasma::Label(this);
    m_servicesLbl->setText("Services:");
    m_statsLayout->addItem(m_servicesLbl, 2, 0);

    m_services = new Plasma::Label(this);
    m_services->setText("");
    m_statsLayout->addItem(m_services, 2, 1);

    m_okLbl = new Plasma::Label(this);
    m_okLbl->setText("OK:");
    m_statsLayout->addItem(m_okLbl, 3, 0);

    m_ok = new Plasma::Label(this);
    m_ok->setText("");
    m_statsLayout->addItem(m_ok, 3, 1);

    m_warningLbl = new Plasma::Label(this);
    m_warningLbl->setText("Warning:");
    m_statsLayout->addItem(m_warningLbl, 4, 0);

    m_warning = new Plasma::Label(this);
    m_warning->setText("");
    m_statsLayout->addItem(m_warning, 4, 1);

    m_criticalLbl = new Plasma::Label(this);
    m_criticalLbl->setText("Critical:");
    m_statsLayout->addItem(m_criticalLbl, 5, 0);

    m_critical = new Plasma::Label(this);
    m_critical->setText("");
    m_statsLayout->addItem(m_critical, 5, 1);

    m_unknownLbl = new Plasma::Label(this);
    m_unknownLbl->setText("Unknown:");
    m_statsLayout->addItem(m_unknownLbl, 6, 0, 1, 1);

    m_unknown = new Plasma::Label(this);
    m_unknown->setText("");
    m_statsLayout->addItem(m_unknown, 6, 1, 1, 1);

    m_layout->addItem(m_statsLayout);

    m_logLbl = new Plasma::Label(this);
    m_logLbl->setText("Service Status");
    m_logLbl->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_logLayout->addItem(m_logLbl);

    m_logWg = new QGraphicsWidget(this);
    m_logListLayout = new QGraphicsLinearLayout(Qt::Vertical, m_logWg);
    m_logListLayout->setSpacing(0);

    m_logScroll = new Plasma::ScrollWidget(this);
    m_logScroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_logScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_logScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_logScroll->setWidget(m_logWg);
    m_logLayout->addItem(m_logScroll);

    m_layout->addItem(m_logLayout);
    setLayout(m_layout);

    updateSize();
}

void ExtendedStatus::addService(QGraphicsLayoutItem* item)
{
    item->setParentLayoutItem(m_logWg);
    m_logListLayout->addItem(item);
}

void ExtendedStatus::clearServices()
{
    QGraphicsLayoutItem* li;
    for (int i = m_logListLayout->count() - 1; i >= 0; --i) {
        li = m_logListLayout->itemAt(i);
        m_logListLayout->removeAt(i);
        delete li;
    }
}

void ExtendedStatus::setStats(const QDateTime& lastChange, const int hosts, const int services, const int ok, const int warning, const int critical, const int unknown)
{
    m_lastUpdate->setText(KGlobal::locale()->formatDateTime(lastChange, KLocale::FancyLongDate, true));
    m_hosts->setText(QString::number(hosts));
    m_services->setText(QString::number(services));
    m_ok->setText(QString::number(ok));
    m_warning->setText(QString::number(warning));
    m_critical->setText(QString::number(critical));
    m_unknown->setText(QString::number(unknown));
}

void ExtendedStatus::updateSize()
{
    //setMinimumSize(m_layout->sizeHint(Qt::MinimumSize));
    //setPreferredSize(m_layout->sizeHint(Qt::PreferredSize));
    //setMaximumSize(m_layout->sizeHint(Qt::MaximumSize));
    update();
}
