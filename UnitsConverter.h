/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#ifndef UNITSCONVERTER_H_
#define UNITSCONVERTER_H_

#include "Dpi.h"
#include "Units.h"
#include <QTransform>

class UnitsConverter
{
public:
	UnitsConverter() = default;
	explicit UnitsConverter(Dpi const& dpi);

	void convert(double& horizontalValue, double& verticalValue, Units fromUnits, Units toUnits) const;
	QTransform transform(Units fromUnits, Units toUnits) const;

	Dpi const& getDpi() const { return m_dpi; }
	void setDpi(Dpi const& dpi) { m_dpi = dpi; }

private:
	Dpi m_dpi;
};

#endif
