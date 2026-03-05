/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich, 4lex4 (Integration from ScanTailor Advanced - GPL-3.0)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#include "OrderByDeviationProvider.h"

OrderByDeviationProvider::OrderByDeviationProvider(DeviationProvider<PageId> const& deviationProvider)
	: m_deviationProvider(&deviationProvider)
{
}

bool
OrderByDeviationProvider::precedes(
	PageId const& lhs_page, bool lhs_incomplete,
	PageId const& rhs_page, bool rhs_incomplete) const
{
	if (lhs_incomplete != rhs_incomplete) {
		return lhs_incomplete; // Incomplete (unknown) go to the back
	}
	return (m_deviationProvider->getDeviationValue(lhs_page) > m_deviationProvider->getDeviationValue(rhs_page));
}
