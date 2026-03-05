/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#include "UnitsConverter.h"
#include "Dpm.h"

UnitsConverter::UnitsConverter(Dpi const& dpi) : m_dpi(dpi) {}

void UnitsConverter::convert(double& horizontalValue, double& verticalValue, Units fromUnits, Units toUnits) const
{
	if (m_dpi.isNull() || fromUnits == toUnits) {
		return;
	}

	Dpm dpm(m_dpi);

	switch (fromUnits) {
		case PIXELS:
			switch (toUnits) {
				case MILLIMETRES:
					horizontalValue = horizontalValue / dpm.horizontal() * 1000.0;
					verticalValue = verticalValue / dpm.vertical() * 1000.0;
					break;
				case CENTIMETRES:
					horizontalValue = horizontalValue / dpm.horizontal() * 100.0;
					verticalValue = verticalValue / dpm.vertical() * 100.0;
					break;
				case INCHES:
					horizontalValue /= m_dpi.horizontal();
					verticalValue /= m_dpi.vertical();
					break;
				default:
					break;
			}
			break;
		case MILLIMETRES:
			switch (toUnits) {
				case PIXELS:
					horizontalValue = horizontalValue / 1000.0 * dpm.horizontal();
					verticalValue = verticalValue / 1000.0 * dpm.vertical();
					break;
				case CENTIMETRES:
					horizontalValue /= 10.0;
					verticalValue /= 10.0;
					break;
				case INCHES:
					horizontalValue = horizontalValue / 1000.0 * dpm.horizontal() / m_dpi.horizontal();
					verticalValue = verticalValue / 1000.0 * dpm.vertical() / m_dpi.vertical();
					break;
				default:
					break;
			}
			break;
		case CENTIMETRES:
			switch (toUnits) {
				case PIXELS:
					horizontalValue = horizontalValue / 100.0 * dpm.horizontal();
					verticalValue = verticalValue / 100.0 * dpm.vertical();
					break;
				case MILLIMETRES:
					horizontalValue *= 10.0;
					verticalValue *= 10.0;
					break;
				case INCHES:
					horizontalValue = horizontalValue / 100.0 * dpm.horizontal() / m_dpi.horizontal();
					verticalValue = verticalValue / 100.0 * dpm.vertical() / m_dpi.vertical();
					break;
				default:
					break;
			}
			break;
		case INCHES:
			switch (toUnits) {
				case PIXELS:
					horizontalValue *= m_dpi.horizontal();
					verticalValue *= m_dpi.vertical();
					break;
				case MILLIMETRES:
					horizontalValue = horizontalValue * m_dpi.horizontal() / dpm.horizontal() * 1000.0;
					verticalValue = verticalValue * m_dpi.vertical() / dpm.vertical() * 1000.0;
					break;
				case CENTIMETRES:
					horizontalValue = horizontalValue * m_dpi.horizontal() / dpm.horizontal() * 100.0;
					verticalValue = verticalValue * m_dpi.vertical() / dpm.vertical() * 100.0;
					break;
				default:
					break;
			}
			break;
	}
}

QTransform UnitsConverter::transform(Units fromUnits, Units toUnits) const
{
	double xScaleFactor = 1.0;
	double yScaleFactor = 1.0;
	convert(xScaleFactor, yScaleFactor, fromUnits, toUnits);
	return QTransform().scale(xScaleFactor, yScaleFactor);
}
