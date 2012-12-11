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

#include <QtGui/QWidget>
#include <QtGui/QGraphicsLayoutItem>
#include <QtGui/QListWidgetItem>

#include <KConfig>
#include <KConfigGroup>

#include <Plasma/DataEngine>
#include <Plasma/PopupApplet>

#include "extendedstatus.h"

class QSizeF;

class IcingaPlasmoid : public Plasma::PopupApplet
{
    Q_OBJECT

public:
    IcingaPlasmoid(QObject* parent, const QVariantList& args);
    virtual ~IcingaPlasmoid();
    void init();
    void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

    QGraphicsWidget *graphicsWidget();
    static const QColor CLR_OK;
    static const QColor CLR_WARNING;
    static const QColor CLR_CRITICAL;
    static const QColor CLR_UNKNOWN;
    static const QColor CLR_UNINITIALIZED;

Q_SIGNALS:
    void dataUpdated();
    void popupUpdated();

protected Q_SLOTS:
    void constraintsEvent(Plasma::Constraints constraints);
    void dataUpdated(const QString& source, const Plasma::DataEngine::Data& data);
    void resetSize();

private Q_SLOTS:
    void updatePopup();
    void updateSize();

private:
    QGraphicsLayoutItem* createLogItem(const QString text, const QString tooltip, const QColor color);
    void expandFontToMax();
    void unset();

    int m_statusOk;
    int m_statusWarning;
    int m_statusCritical;
    int m_statusUnknown;
    int m_realstatus;
    int m_servicesCount;
    int m_hostsCount;
    QVariantList m_msgOk;
    QVariantList m_msgWarning;
    QVariantList m_msgCritical;
    QVariantList m_msgUnknown;
    QString m_msgError;
    QDateTime m_lastUpdate;
    KConfig m_config;
    KConfigGroup m_generalcg;
    QString m_text;
    QFont m_font;
    ExtendedStatus* m_popup;
};

const QColor IcingaPlasmoid::CLR_OK(0, 255, 0, 64);
const QColor IcingaPlasmoid::CLR_WARNING(255, 255, 0, 64);
const QColor IcingaPlasmoid::CLR_CRITICAL(255, 0, 0, 64);
const QColor IcingaPlasmoid::CLR_UNKNOWN(255, 0, 255, 64);
const QColor IcingaPlasmoid::CLR_UNINITIALIZED(0, 0, 255, 64);


#endif // ICINGAMONITOR_H
