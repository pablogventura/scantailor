/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#ifndef UNITS_H_
#define UNITS_H_

#include <QString>

enum Units
{
	PIXELS,
	MILLIMETRES,
	CENTIMETRES,
	INCHES
};

QString unitsToString(Units units);
Units unitsFromString(QString const& string);
QString unitsToLocalizedString(Units units);

#endif
