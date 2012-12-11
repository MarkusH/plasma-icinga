/*
    Icinga Data Engine continuously retieves data from an Icinga instance
    Copyright (C) 2012  Markus Holtermann <info@markusholtermann.eu>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
   (at your option)any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "icingaplasmoid.h"

#include <QtGui/QPainter>

#include <KGlobalSettings>

#include <Plasma/IconWidget>

IcingaPlasmoid::IcingaPlasmoid(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
      m_config("icingarc"),
      m_text("000/00/00/00\nH:00/S:000")
{
    qDebug() << "constructor";
    setBackgroundHints(DefaultBackground);
    m_generalcg = m_config.group("General");
    unset();
    resize(100, 100);
    connect(this, SIGNAL(dataUpdated()), this, SLOT(updatePopup()));
    connect(this, SIGNAL(dataUpdated()), this, SLOT(updateSize()));

    m_popup = new ExtendedStatus(this);
    connect(this, SIGNAL(popupUpdated()), m_popup, SLOT(updateSize()));
}

IcingaPlasmoid::~IcingaPlasmoid()
{
}

/*
 * Taken from the kde-workspace digital clock applet at rev 45bd3bfb9db950dde7cd09e5707629224275b199 and adjusted to special needs
 */
void IcingaPlasmoid::constraintsEvent(Plasma::Constraints constraints)
{
    Plasma::Applet::constraintsEvent(constraints);

    if (constraints & Plasma::SizeConstraint || constraints & Plasma::FormFactorConstraint) {
        updateSize();
    }
}

QGraphicsLayoutItem* IcingaPlasmoid::createLogItem(const QString text, const QString tooltip, const QColor color) {
    qDebug() << "createLogItem" << text << tooltip << color;
    Plasma::IconWidget* item = new Plasma::IconWidget();
    item->setText(text);
    item->setDrawBackground(true);
    item->setTextBackgroundColor(color);
    item->setToolTip(tooltip);
    item->setPreferredIconSize(QSizeF(0, 0));
    return item;
}

void IcingaPlasmoid::init()
{
    qDebug() << "init";
    Plasma::Applet::init();
    dataEngine("icinga")->connectSource("all", this, m_generalcg.readEntry<uint>("interval", 300000));
}

void IcingaPlasmoid::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
{
    qDebug() << "dataUpdated" << source << data;
    Q_UNUSED(source)
    if (data["connected"].toBool() == true) {
        qDebug() << "dataUpdated: connected";
        m_statusOk = data["status-ok"].toInt();
        m_statusWarning = data["status-warning"].toInt();
        m_statusCritical = data["status-critical"].toInt();
        m_statusUnknown = data["status-unknown"].toInt();
        // make sure that the value is -1 if the key is no realstatus is set
        m_realstatus = data.contains("realstatus") ? data["realstatus"].toInt() : -1;
        m_servicesCount = data["services-count"].toInt();
        m_hostsCount = data["hosts-count"].toInt();
        m_lastUpdate = data["last-update"].toDateTime();
        m_msgOk = data["msg-ok"].toList();
        m_msgWarning = data["msg-warning"].toList();
        m_msgCritical = data["msg-critical"].toList();
        m_msgUnknown = data["msg-unknown"].toList();
    } else {
        qDebug() << "dataUpdated: not connected";
        unset();
        m_msgError = data["error-msg"].toString();
    }
    emit dataUpdated();
}

/*
 * Taken from the kde-workspace digital clock applet at rev 45bd3bfb9db950dde7cd09e5707629224275b199
 */
void IcingaPlasmoid::expandFontToMax()
{
    bool first = true;
    const QRect rect = contentsRect().toRect();

    // Starting with the given font, increase its size until it'll fill the rect
    do {
        if (first) {
            first = false;
        } else  {
            m_font.setPointSize(m_font.pointSize() + 1);
        }

        const QFontMetrics fm(m_font);
        QRect fr = fm.boundingRect(rect, 0, m_text);
        if (fr.width() >= rect.width() || fr.height() >= rect.height()) {
            break;
        }
    } while (true);
}

QGraphicsWidget* IcingaPlasmoid::graphicsWidget()
{
    qDebug() << "graphicsWidget";
    return m_popup;
}

void IcingaPlasmoid::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(option)
    QColor bgcolor, brdcolor;
    switch (m_realstatus) {
    case 0:
        bgcolor = CLR_OK;
        break;
    case 1:
        bgcolor = CLR_WARNING;
        break;
    case 2:
        bgcolor = CLR_CRITICAL;
        break;
    case 3:
        bgcolor = CLR_UNKNOWN;
        break;
    default:
        bgcolor = CLR_UNINITIALIZED;
    }
    brdcolor = bgcolor;
    brdcolor.setAlpha(192);
    painter->setBrush(bgcolor);
    painter->setPen(brdcolor);
    const qreal radius = qMin<int>(contentsRect.height(), contentsRect.width()) * 0.1;
    painter->drawRoundedRect(contentsRect, radius, radius);
    painter->setPen(Qt::black);
    painter->setFont(m_font);
    painter->drawText(contentsRect, Qt::AlignCenter, m_text);
}

/*
 * Taken from the kde-workspace digital clock applet at rev 45bd3bfb9db950dde7cd09e5707629224275b199
 */
void IcingaPlasmoid::resetSize()
{
    constraintsEvent(Plasma::SizeConstraint);
}

void IcingaPlasmoid::unset()
{
    qDebug() << "unset";
    m_statusOk = -1;
    m_statusWarning = -1;
    m_statusCritical = -1;
    m_statusUnknown = -1;
    m_realstatus = -1;
    m_servicesCount = -1;
    m_hostsCount = 0;
    m_msgOk = QVariantList();
    m_msgWarning = QVariantList();
    m_msgCritical = QVariantList();
    m_msgUnknown = QVariantList();
    m_msgError = "";
    m_lastUpdate = QDateTime::fromMSecsSinceEpoch(0);
}

void IcingaPlasmoid::updatePopup()
{
    qDebug() << "updatePopup";

    m_popup->clearServices();
    m_popup->setStats(m_lastUpdate, m_hostsCount, m_servicesCount, m_statusOk, m_statusWarning, m_statusCritical, m_statusUnknown);
    if (m_msgError != "") {
        m_popup->addService(createLogItem(m_msgError, "", CLR_UNINITIALIZED));
    }

    QVariantMap msg;
    foreach(QVariant _msg, m_msgUnknown) {
        msg = _msg.toMap();
        m_popup->addService(createLogItem(msg["msg"].toString(), msg["host"].toString() + ": " + msg["service"].toString(), CLR_UNKNOWN));
    }

    foreach(QVariant _msg, m_msgCritical) {
        msg = _msg.toMap();
        m_popup->addService(createLogItem(msg["msg"].toString(), msg["host"].toString() + ": " + msg["service"].toString(), CLR_CRITICAL));
    }

    foreach(QVariant _msg, m_msgWarning) {
        msg = _msg.toMap();
        m_popup->addService(createLogItem(msg["msg"].toString(), msg["host"].toString() + ": " + msg["service"].toString(), CLR_WARNING));
    }

//     foreach(QVariant _msg, m_msgOk) {
//         msg = _msg.toMap();
//         m_popup->addService(createLogItem(msg, CLR_OK));
//     }
    emit popupUpdated();
}

/*
 * Taken from the kde-workspace digital clock applet at rev 45bd3bfb9db950dde7cd09e5707629224275b199 and adjusted to special needs
 */
void IcingaPlasmoid::updateSize()
{
    Plasma::FormFactor ff = formFactor();

    m_text = QString().sprintf("%d/%d/%d/%d\nH:%d/S:%d", m_statusOk, m_statusWarning, m_statusCritical, m_statusUnknown, m_hostsCount, m_servicesCount);
    if (ff != Plasma::Vertical && ff != Plasma::Horizontal) {
        const QFontMetricsF metrics(KGlobalSettings::smallestReadableFont());
        setMinimumSize(metrics.size(0, m_text));
    }

    // more magic numbers
    int aspect = 2;

    int w, h;
    m_font = KGlobalSettings::smallestReadableFont();
    const QFontMetrics metrics(m_font);
    // if there's enough vertical space, wrap the words
    if (contentsRect().height() < m_font.pointSize() * 6) {
        const QSize s = metrics.size(0, m_text);
        w = s.width() + metrics.width(" ");
        h = m_font.pointSize();
    } else {
        const QSize s = metrics.size(0, m_text);
        w = s.width();
        h = m_font.pointSize();
    }

    w = qMax(w, (int)(contentsRect().height() * aspect));
    h = h+(int)(contentsRect().width() / aspect);

    if (ff == Plasma::Horizontal) {
        // We have a fixed height, set some sensible width
        setMinimumSize(QSize(w+5, 0));
    } else {
        // We have a fixed width, set some sensible height
        setMinimumSize(QSize(0, h));
    }

    setPreferredSize(QSize(w+5, h));
    emit sizeHintChanged(Qt::PreferredSize);

    expandFontToMax();

    //generatePixmap();
    update();
}

K_EXPORT_PLASMA_APPLET(icinga_plasmoid, IcingaPlasmoid)

#include "icingaplasmoid.moc"
