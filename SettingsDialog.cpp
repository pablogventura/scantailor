/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SettingsDialog.h"
#include "ApplicationSettings.h"
#include "OpenGLSupport.h"
#include "Units.h"
#include "UnitsProvider.h"
#include "config.h"
#include <QMessageBox>
#include <QSettings>
#include <QVariant>
#include <tiff.h>

SettingsDialog::SettingsDialog(QWidget* parent)
:	QDialog(parent)
{
	ui.setupUi(this);

	ApplicationSettings& appSettings = ApplicationSettings::getInstance();

#ifndef ENABLE_OPENGL
	ui.use3DAcceleration->setChecked(false);
	ui.use3DAcceleration->setEnabled(false);
	ui.use3DAcceleration->setToolTip(tr("Compiled without OpenGL support."));
#else
	if (!OpenGLSupport::supported()) {
		ui.use3DAcceleration->setChecked(false);
		ui.use3DAcceleration->setEnabled(false);
		ui.use3DAcceleration->setToolTip(tr("Your hardware / driver don't provide the necessary features."));
	} else {
		ui.use3DAcceleration->setChecked(appSettings.isOpenGlEnabled());
	}
#endif

	ui.colorSchemeBox->addItem(tr("Dark"), QStringLiteral("dark"));
	ui.colorSchemeBox->addItem(tr("Light"), QStringLiteral("light"));
	int schemeIdx = ui.colorSchemeBox->findData(appSettings.getColorScheme());
	ui.colorSchemeBox->setCurrentIndex(schemeIdx >= 0 ? schemeIdx : 0);

	ui.autoSaveProjectCB->setChecked(appSettings.isAutoSaveProjectEnabled());

	ui.unitsBox->addItem(tr("px"), QStringLiteral("px"));
	ui.unitsBox->addItem(tr("mm"), QStringLiteral("mm"));
	ui.unitsBox->addItem(tr("cm"), QStringLiteral("cm"));
	ui.unitsBox->addItem(tr("in"), QStringLiteral("in"));
	int unitsIdx = ui.unitsBox->findData(appSettings.getUnits());
	ui.unitsBox->setCurrentIndex(unitsIdx >= 0 ? unitsIdx : 1);

	ui.batchThreadCountSB->setValue(appSettings.getBatchThreadCount());

	ui.blackOnWhiteDetectionCB->setChecked(appSettings.isBlackOnWhiteDetectionEnabled());
	ui.blackOnWhiteAtOutputCB->setChecked(appSettings.isBlackOnWhiteDetectionOutputEnabled());
	ui.blackOnWhiteAtOutputCB->setEnabled(ui.blackOnWhiteDetectionCB->isChecked());
	connect(ui.blackOnWhiteDetectionCB, SIGNAL(clicked(bool)), SLOT(blackOnWhiteDetectionToggled(bool)));

	ui.thumbnailQualitySB->setValue(appSettings.getThumbnailQuality().width());
	ui.thumbnailSizeSB->setValue((int)appSettings.getMaxLogicalThumbnailSize().width());
	ui.singleColumnThumbnailsCB->setChecked(appSettings.isSingleColumnThumbnailDisplayEnabled());
	ui.highlightDeviationCB->setChecked(appSettings.isHighlightDeviationEnabled());
	ui.cancelingSelectionQuestionCB->setChecked(appSettings.isCancelingSelectionQuestionEnabled());

	ui.tiffCompressionBWBox->addItem(tr("None"), COMPRESSION_NONE);
	ui.tiffCompressionBWBox->addItem(tr("LZW"), COMPRESSION_LZW);
	ui.tiffCompressionBWBox->addItem(tr("Deflate"), COMPRESSION_DEFLATE);
	ui.tiffCompressionBWBox->addItem(tr("CCITT G4"), COMPRESSION_CCITTFAX4);
	int bwIdx = ui.tiffCompressionBWBox->findData(appSettings.getTiffBwCompression());
	ui.tiffCompressionBWBox->setCurrentIndex(bwIdx >= 0 ? bwIdx : 3);

	ui.tiffCompressionColorBox->addItem(tr("None"), COMPRESSION_NONE);
	ui.tiffCompressionColorBox->addItem(tr("LZW"), COMPRESSION_LZW);
	ui.tiffCompressionColorBox->addItem(tr("Deflate"), COMPRESSION_DEFLATE);
	ui.tiffCompressionColorBox->addItem(tr("JPEG"), COMPRESSION_JPEG);
	int colorIdx = ui.tiffCompressionColorBox->findData(appSettings.getTiffColorCompression());
	ui.tiffCompressionColorBox->setCurrentIndex(colorIdx >= 0 ? colorIdx : 1);

	connect(ui.buttonBox, SIGNAL(accepted()), SLOT(commitChanges()));
}

SettingsDialog::~SettingsDialog()
{
}

void
SettingsDialog::commitChanges()
{
	ApplicationSettings& s = ApplicationSettings::getInstance();

#ifdef ENABLE_OPENGL
	s.setOpenGlEnabled(ui.use3DAcceleration->isChecked());
#endif

	QString newScheme = ui.colorSchemeBox->currentData().toString();
	if (newScheme != s.getColorScheme()) {
		s.setColorScheme(newScheme);
		QMessageBox::information(this, tr("Information"),
			tr("Restart the application to apply the color scheme."));
	}

	s.setAutoSaveProjectEnabled(ui.autoSaveProjectCB->isChecked());

	QString newUnits = ui.unitsBox->currentData().toString();
	s.setUnits(newUnits);
	UnitsProvider::getInstance().setUnits(unitsFromString(newUnits));

	s.setBatchThreadCount(ui.batchThreadCountSB->value());

	s.setBlackOnWhiteDetectionEnabled(ui.blackOnWhiteDetectionCB->isChecked());
	s.setBlackOnWhiteDetectionOutputEnabled(ui.blackOnWhiteAtOutputCB->isChecked());

	s.setThumbnailQuality(QSize(ui.thumbnailQualitySB->value(), ui.thumbnailQualitySB->value()));
	s.setMaxLogicalThumbnailSize(QSizeF(ui.thumbnailSizeSB->value(),
		(int)(ui.thumbnailSizeSB->value() * 16.0 / 25.0)));
	s.setSingleColumnThumbnailDisplayEnabled(ui.singleColumnThumbnailsCB->isChecked());
	s.setHighlightDeviationEnabled(ui.highlightDeviationCB->isChecked());
	s.setCancelingSelectionQuestionEnabled(ui.cancelingSelectionQuestionCB->isChecked());

	s.setTiffBwCompression(ui.tiffCompressionBWBox->currentData().toInt());
	s.setTiffColorCompression(ui.tiffCompressionColorBox->currentData().toInt());
}

void
SettingsDialog::blackOnWhiteDetectionToggled(bool checked)
{
	ui.blackOnWhiteAtOutputCB->setEnabled(checked);
}
