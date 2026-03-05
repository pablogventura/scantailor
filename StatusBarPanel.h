/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#ifndef STATUSBARPANEL_H_
#define STATUSBARPANEL_H_

#include "UnitsListener.h"
#include "ui_StatusBarPanel.h"
#include <QWidget>

class PageId;

class StatusBarPanel : public QWidget, public UnitsListener
{
	Q_OBJECT
public:
	StatusBarPanel();
	~StatusBarPanel() override = default;

	void updatePage(int pageNumber, int pageCount, PageId const& pageId);
	void clear();

	void onUnitsChanged(Units) override;

private:
	Ui::StatusBarPanel ui;
};

#endif
