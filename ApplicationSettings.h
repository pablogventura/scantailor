/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#ifndef APPLICATIONSETTINGS_H_
#define APPLICATIONSETTINGS_H_

#include "foundation/NonCopyable.h"
#include <QSettings>
#include <QSize>
#include <QSizeF>
#include <QString>

class ApplicationSettings
{
	DECLARE_NON_COPYABLE(ApplicationSettings)
public:
	static ApplicationSettings& getInstance();

	bool isOpenGlEnabled() const;
	void setOpenGlEnabled(bool enabled);

	QString getColorScheme() const;
	void setColorScheme(QString const& scheme);

	bool isAutoSaveProjectEnabled() const;
	void setAutoSaveProjectEnabled(bool enabled);

	int getTiffBwCompression() const;
	void setTiffBwCompression(int compression);

	int getTiffColorCompression() const;
	void setTiffColorCompression(int compression);

	bool isBlackOnWhiteDetectionEnabled() const;
	void setBlackOnWhiteDetectionEnabled(bool enabled);

	bool isBlackOnWhiteDetectionOutputEnabled() const;
	void setBlackOnWhiteDetectionOutputEnabled(bool enabled);

	bool isHighlightDeviationEnabled() const;
	void setHighlightDeviationEnabled(bool enabled);

	double getDeskewDeviationCoef() const;
	void setDeskewDeviationCoef(double value);

	double getDeskewDeviationThreshold() const;
	void setDeskewDeviationThreshold(double value);

	double getSelectContentDeviationCoef() const;
	void setSelectContentDeviationCoef(double value);

	double getSelectContentDeviationThreshold() const;
	void setSelectContentDeviationThreshold(double value);

	double getMarginsDeviationCoef() const;
	void setMarginsDeviationCoef(double value);

	double getMarginsDeviationThreshold() const;
	void setMarginsDeviationThreshold(double value);

	QSize getThumbnailQuality() const;
	void setThumbnailQuality(QSize const& quality);

	QSizeF getMaxLogicalThumbnailSize() const;
	void setMaxLogicalThumbnailSize(QSizeF const& size);

	bool isSingleColumnThumbnailDisplayEnabled() const;
	void setSingleColumnThumbnailDisplayEnabled(bool enabled);

	QString getLanguage() const;
	void setLanguage(QString const& language);

	QString getUnits() const;
	void setUnits(QString const& units);

	QString getCurrentProfile() const;
	void setCurrentProfile(QString const& profile);

	bool isCancelingSelectionQuestionEnabled() const;
	void setCancelingSelectionQuestionEnabled(bool enabled);

	int getBatchThreadCount() const;
	void setBatchThreadCount(int count);

private:
	ApplicationSettings();

	static QString getKey(QString const& keyName);

	static QString const ROOT_KEY;
	static QString const OPENGL_STATE_KEY;
	static QString const AUTO_SAVE_PROJECT_KEY;
	static QString const COLOR_SCHEME_KEY;
	static QString const TIFF_BW_COMPRESSION_KEY;
	static QString const TIFF_COLOR_COMPRESSION_KEY;
	static QString const BLACK_ON_WHITE_DETECTION_KEY;
	static QString const BLACK_ON_WHITE_DETECTION_OUTPUT_KEY;
	static QString const HIGHLIGHT_DEVIATION_KEY;
	static QString const DESKEW_DEVIATION_COEF_KEY;
	static QString const DESKEW_DEVIATION_THRESHOLD_KEY;
	static QString const SELECT_CONTENT_DEVIATION_COEF_KEY;
	static QString const SELECT_CONTENT_DEVIATION_THRESHOLD_KEY;
	static QString const MARGINS_DEVIATION_COEF_KEY;
	static QString const MARGINS_DEVIATION_THRESHOLD_KEY;
	static QString const THUMBNAIL_QUALITY_KEY;
	static QString const MAX_LOGICAL_THUMBNAIL_SIZE_KEY;
	static QString const SINGLE_COLUMN_THUMBNAIL_DISPLAY_KEY;
	static QString const LANGUAGE_KEY;
	static QString const UNITS_KEY;
	static QString const CURRENT_PROFILE_KEY;
	static QString const SHOW_CANCELING_SELECTION_QUESTION_KEY;
	static QString const BATCH_THREAD_COUNT_KEY;

	QSettings m_settings;
};

#endif
