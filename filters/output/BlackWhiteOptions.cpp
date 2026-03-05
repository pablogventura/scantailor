/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BlackWhiteOptions.h"
#include <QDomDocument>
#include <QDomElement>
#include <QString>

namespace output
{

BlackWhiteOptions::BlackWhiteOptions()
:	m_thresholdAdjustment(0),
	m_binarizationMethod(OTSU),
	m_windowSize(25),
	m_sauvolaCoef(0.34),
	m_wolfLowerBound(1),
	m_wolfUpperBound(254),
	m_wolfCoef(0.3)
{
}

BlackWhiteOptions::BlackWhiteOptions(QDomElement const& el)
:	m_thresholdAdjustment(el.attribute("thresholdAdj").toInt()),
	m_binarizationMethod(parseBinarizationMethod(el.attribute("method", "otsu"))),
	m_windowSize(25),
	m_sauvolaCoef(0.34),
	m_wolfLowerBound(1),
	m_wolfUpperBound(254),
	m_wolfCoef(0.3)
{
	QString const ws(el.attribute("windowSize"));
	if (!ws.isEmpty()) m_windowSize = ws.toInt();
	QString const sc(el.attribute("sauvolaCoef"));
	if (!sc.isEmpty()) m_sauvolaCoef = sc.toDouble();
	QString const wlb(el.attribute("wolfLowerBound"));
	if (!wlb.isEmpty()) m_wolfLowerBound = wlb.toInt();
	QString const wub(el.attribute("wolfUpperBound"));
	if (!wub.isEmpty()) m_wolfUpperBound = wub.toInt();
	QString const wc(el.attribute("wolfCoef"));
	if (!wc.isEmpty()) m_wolfCoef = wc.toDouble();
	if (m_windowSize < 3) m_windowSize = 3;
	if (m_windowSize % 2 == 0) ++m_windowSize;
}

QDomElement
BlackWhiteOptions::toXml(QDomDocument& doc, QString const& name) const
{
	QDomElement el(doc.createElement(name));
	el.setAttribute("thresholdAdj", m_thresholdAdjustment);
	el.setAttribute("method", formatBinarizationMethod(m_binarizationMethod));
	el.setAttribute("windowSize", m_windowSize);
	el.setAttribute("sauvolaCoef", m_sauvolaCoef);
	el.setAttribute("wolfLowerBound", m_wolfLowerBound);
	el.setAttribute("wolfUpperBound", m_wolfUpperBound);
	el.setAttribute("wolfCoef", m_wolfCoef);
	return el;
}

BinarizationMethod
BlackWhiteOptions::parseBinarizationMethod(QString const& str)
{
	if (str == QLatin1String("sauvola")) return SAUVOLA;
	if (str == QLatin1String("wolf")) return WOLF;
	return OTSU;
}

QString
BlackWhiteOptions::formatBinarizationMethod(BinarizationMethod method)
{
	switch (method) {
		case SAUVOLA: return QString::fromLatin1("sauvola");
		case WOLF: return QString::fromLatin1("wolf");
		default: return QString::fromLatin1("otsu");
	}
}

bool
BlackWhiteOptions::operator==(BlackWhiteOptions const& other) const
{
	return m_thresholdAdjustment == other.m_thresholdAdjustment
		&& m_binarizationMethod == other.m_binarizationMethod
		&& m_windowSize == other.m_windowSize
		&& m_sauvolaCoef == other.m_sauvolaCoef
		&& m_wolfLowerBound == other.m_wolfLowerBound
		&& m_wolfUpperBound == other.m_wolfUpperBound
		&& m_wolfCoef == other.m_wolfCoef;
}

bool
BlackWhiteOptions::operator!=(BlackWhiteOptions const& other) const
{
	return !(*this == other);
}

} // namespace output
