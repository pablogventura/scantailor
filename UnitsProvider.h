/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#ifndef UNITSPROVIDER_H_
#define UNITSPROVIDER_H_

#include "foundation/NonCopyable.h"
#include "Units.h"
#include "UnitsListener.h"
#include <list>

class Dpi;

class UnitsProvider
{
	DECLARE_NON_COPYABLE(UnitsProvider)
public:
	static UnitsProvider& getInstance();

	Units getUnits() const { return m_units; }
	void setUnits(Units units);

	void addListener(UnitsListener* listener);
	void removeListener(UnitsListener* listener);

	void convertFrom(double& horizontalValue, double& verticalValue, Units fromUnits, Dpi const& dpi) const;
	void convertTo(double& horizontalValue, double& verticalValue, Units toUnits, Dpi const& dpi) const;

protected:
	void unitsChanged();

private:
	UnitsProvider();

	std::list<UnitsListener*> m_unitsListeners;
	Units m_units;
};

#endif
