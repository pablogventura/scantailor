/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#include "UnitsListener.h"
#include "UnitsProvider.h"

UnitsListener::UnitsListener()
{
	UnitsProvider::getInstance().addListener(this);
}

UnitsListener::~UnitsListener()
{
	UnitsProvider::getInstance().removeListener(this);
}
