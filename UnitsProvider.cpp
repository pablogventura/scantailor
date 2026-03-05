/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#include "UnitsProvider.h"
#include "ApplicationSettings.h"
#include "Dpm.h"
#include "UnitsConverter.h"
#include <algorithm>

UnitsProvider::UnitsProvider() : m_units(unitsFromString(ApplicationSettings::getInstance().getUnits())) {}

UnitsProvider& UnitsProvider::getInstance()
{
	static UnitsProvider instance;
	return instance;
}

void UnitsProvider::setUnits(Units units)
{
	m_units = units;
	ApplicationSettings::getInstance().setUnits(unitsToString(units));
	unitsChanged();
}

void UnitsProvider::addListener(UnitsListener* listener)
{
	m_unitsListeners.push_back(listener);
}

void UnitsProvider::removeListener(UnitsListener* listener)
{
	m_unitsListeners.remove(listener);
}

void UnitsProvider::unitsChanged()
{
	for (UnitsListener* listener : m_unitsListeners) {
		listener->onUnitsChanged(m_units);
	}
}

void UnitsProvider::convertFrom(double& horizontalValue, double& verticalValue, Units fromUnits, Dpi const& dpi) const
{
	UnitsConverter(dpi).convert(horizontalValue, verticalValue, fromUnits, m_units);
}

void UnitsProvider::convertTo(double& horizontalValue, double& verticalValue, Units toUnits, Dpi const& dpi) const
{
	UnitsConverter(dpi).convert(horizontalValue, verticalValue, m_units, toUnits);
}
