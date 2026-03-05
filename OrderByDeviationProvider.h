/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich, 4lex4 (Integration from ScanTailor Advanced - GPL-3.0)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#ifndef ORDERBYDEVIATIONPROVIDER_H_
#define ORDERBYDEVIATIONPROVIDER_H_

#include "DeviationProvider.h"
#include "PageId.h"
#include "PageOrderProvider.h"

class OrderByDeviationProvider : public PageOrderProvider
{
public:
	explicit OrderByDeviationProvider(DeviationProvider<PageId> const& deviationProvider);

	bool precedes(
		PageId const& lhs_page, bool lhs_incomplete,
		PageId const& rhs_page, bool rhs_incomplete) const override;

private:
	DeviationProvider<PageId> const* m_deviationProvider;
};

#endif
