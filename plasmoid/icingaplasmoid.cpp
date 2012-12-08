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

IcingaPlasmoid::IcingaPlasmoid(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_statusOk(-1),
      m_statusWarning(-1),
      m_statusCritical(-1),
      m_statusUnknown(-1),
      m_realstatus(-1),
      m_servicesCount(-1),
      m_hostsCount(0),
      m_config("icingarc"),
      m_text("000/00/00/00\nH:00/S:000")
{
    setBackgroundHints(DefaultBackground);
    m_generalcg = m_config.group("General");
    resize(100, 100);
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

void IcingaPlasmoid::init()
{
    Plasma::Applet::init();
    dataEngine("icinga")->connectSource("all", this, m_generalcg.readEntry<uint>("interval", 300000));
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

void IcingaPlasmoid::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
{
    Q_UNUSED(source)
    m_statusOk = data["status-ok"].toInt();
    m_statusWarning = data["status-warning"].toInt();
    m_statusCritical = data["status-critical"].toInt();
    m_statusUnknown = data["status-unknown"].toInt();
    m_realstatus = data["realstatus"].toInt();
    m_servicesCount = data["services-count"].toInt();
    m_hostsCount = data["hosts-count"].toInt();
    updateSize();
}

void IcingaPlasmoid::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(option)
    QColor bgcolor, brdcolor;
    switch (m_realstatus) {
      case 0: // OK
	bgcolor = QColor(0, 255, 0, 64);
	break;
      case 1: // WARNING
	bgcolor = QColor(255, 255, 0, 64);
	break;
      case 2: // CRITICAL
	bgcolor = QColor(255, 0, 0, 64);
	break;
      case 3: // UNKNOWN
	bgcolor = QColor(255, 0, 255, 64);
	break;
      default: // UNINITIALIZED
	bgcolor = QColor(0, 0, 255, 64);
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