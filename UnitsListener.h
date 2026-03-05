/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#ifndef UNITSLISTENER_H_
#define UNITSLISTENER_H_

#include "Units.h"

class Dpi;

class UnitsListener
{
protected:
	UnitsListener();

public:
	virtual ~UnitsListener();

	virtual void onUnitsChanged(Units units) = 0;
};

#endif
