/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#include "StatusBarPanel.h"
#include "PageId.h"
#include <QFileInfo>

StatusBarPanel::StatusBarPanel()
{
	ui.setupUi(this);
	clear();
}

void StatusBarPanel::updatePage(int pageNumber, int pageCount, PageId const& pageId)
{
	ui.pageNoLabel->setText(tr("p. %1 / %2").arg(pageNumber).arg(pageCount));
	ui.pageNoLabel->setVisible(true);

	QString pageFileInfo = QFileInfo(pageId.imageId().filePath()).completeBaseName();
	if (pageFileInfo.size() > 15) {
		pageFileInfo = QStringLiteral("...") + pageFileInfo.right(13);
	}
	if (pageId.subPage() != PageId::SINGLE_PAGE) {
		pageFileInfo = pageFileInfo.right(11) + (pageId.subPage() == PageId::LEFT_PAGE ? tr(" [L]") : tr(" [R]"));
	}

	ui.pageInfoLine->setVisible(true);
	ui.pageInfoLabel->setText(pageFileInfo);
	ui.pageInfoLabel->setVisible(true);
}

void StatusBarPanel::clear()
{
	ui.mousePosLabel->clear();
	ui.mousePosLabel->hide();
	ui.physSizeLabel->clear();
	ui.physSizeLabel->hide();
	ui.pageNoLabel->clear();
	ui.pageNoLabel->hide();
	ui.pageInfoLabel->clear();
	ui.pageInfoLabel->hide();
	ui.zoneModeLabel->hide();

	ui.mousePosLine->setVisible(false);
	ui.physSizeLine->setVisible(false);
	ui.pageInfoLine->setVisible(false);
	ui.zoneModeLine->setVisible(false);
}

void StatusBarPanel::onUnitsChanged(Units)
{
	// When we add mouse pos/phys size from ImageViewInfoProvider, refresh here.
}
