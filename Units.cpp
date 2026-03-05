/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#include "Units.h"
#include <QObject>

QString unitsToString(Units units)
{
	switch (units) {
		case PIXELS: return QStringLiteral("px");
		case MILLIMETRES: return QStringLiteral("mm");
		case CENTIMETRES: return QStringLiteral("cm");
		case INCHES: return QStringLiteral("in");
	}
	return QStringLiteral("mm");
}

Units unitsFromString(QString const& string)
{
	if (string == QLatin1String("px")) return PIXELS;
	if (string == QLatin1String("cm")) return CENTIMETRES;
	if (string == QLatin1String("in")) return INCHES;
	return MILLIMETRES;
}

QString unitsToLocalizedString(Units units)
{
	switch (units) {
		case PIXELS: return QObject::tr("px");
		case MILLIMETRES: return QObject::tr("mm");
		case CENTIMETRES: return QObject::tr("cm");
		case INCHES: return QObject::tr("in");
	}
	return QObject::tr("mm");
}
