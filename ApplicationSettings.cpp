/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
    (Integration from ScanTailor Advanced - GPL-3.0)
*/

#include "ApplicationSettings.h"
#include <QLocale>
#include <QSettings>
#include <tiff.h>

QString const ApplicationSettings::ROOT_KEY = QStringLiteral("settings");
QString const ApplicationSettings::OPENGL_STATE_KEY = QStringLiteral("enable_opengl");
QString const ApplicationSettings::AUTO_SAVE_PROJECT_KEY = QStringLiteral("auto_save_project");
QString const ApplicationSettings::COLOR_SCHEME_KEY = QStringLiteral("color_scheme");
QString const ApplicationSettings::TIFF_BW_COMPRESSION_KEY = QStringLiteral("bw_compression");
QString const ApplicationSettings::TIFF_COLOR_COMPRESSION_KEY = QStringLiteral("color_compression");
QString const ApplicationSettings::BLACK_ON_WHITE_DETECTION_KEY = QStringLiteral("black_on_white_detection");
QString const ApplicationSettings::BLACK_ON_WHITE_DETECTION_OUTPUT_KEY = QStringLiteral("black_on_white_detection_at_output");
QString const ApplicationSettings::HIGHLIGHT_DEVIATION_KEY = QStringLiteral("highlight_deviation");
QString const ApplicationSettings::DESKEW_DEVIATION_COEF_KEY = QStringLiteral("deskew_deviation_coef");
QString const ApplicationSettings::DESKEW_DEVIATION_THRESHOLD_KEY = QStringLiteral("deskew_deviation_threshold");
QString const ApplicationSettings::SELECT_CONTENT_DEVIATION_COEF_KEY = QStringLiteral("select_content_deviation_coef");
QString const ApplicationSettings::SELECT_CONTENT_DEVIATION_THRESHOLD_KEY = QStringLiteral("select_content_deviation_threshold");
QString const ApplicationSettings::MARGINS_DEVIATION_COEF_KEY = QStringLiteral("margins_deviation_coef");
QString const ApplicationSettings::MARGINS_DEVIATION_THRESHOLD_KEY = QStringLiteral("margins_deviation_threshold");
QString const ApplicationSettings::THUMBNAIL_QUALITY_KEY = QStringLiteral("thumbnail_quality");
QString const ApplicationSettings::MAX_LOGICAL_THUMBNAIL_SIZE_KEY = QStringLiteral("max_logical_thumb_size");
QString const ApplicationSettings::SINGLE_COLUMN_THUMBNAIL_DISPLAY_KEY = QStringLiteral("single_column_thumbnail_display");
QString const ApplicationSettings::LANGUAGE_KEY = QStringLiteral("language");
QString const ApplicationSettings::UNITS_KEY = QStringLiteral("units");
QString const ApplicationSettings::CURRENT_PROFILE_KEY = QStringLiteral("current_profile");
QString const ApplicationSettings::SHOW_CANCELING_SELECTION_QUESTION_KEY = QStringLiteral("selection_canceling_question");
QString const ApplicationSettings::BATCH_THREAD_COUNT_KEY = QStringLiteral("batch_thread_count");

QString ApplicationSettings::getKey(QString const& keyName)
{
	return ROOT_KEY + QLatin1Char('/') + keyName;
}

ApplicationSettings::ApplicationSettings() = default;

ApplicationSettings& ApplicationSettings::getInstance()
{
	static ApplicationSettings instance;
	return instance;
}

bool ApplicationSettings::isOpenGlEnabled() const
{
	return m_settings.value(getKey(OPENGL_STATE_KEY), false).toBool();
}

void ApplicationSettings::setOpenGlEnabled(bool enabled)
{
	m_settings.setValue(getKey(OPENGL_STATE_KEY), enabled);
}

QString ApplicationSettings::getColorScheme() const
{
	return m_settings.value(getKey(COLOR_SCHEME_KEY), QStringLiteral("dark")).toString();
}

void ApplicationSettings::setColorScheme(QString const& scheme)
{
	m_settings.setValue(getKey(COLOR_SCHEME_KEY), scheme);
}

bool ApplicationSettings::isAutoSaveProjectEnabled() const
{
	return m_settings.value(getKey(AUTO_SAVE_PROJECT_KEY), false).toBool();
}

void ApplicationSettings::setAutoSaveProjectEnabled(bool enabled)
{
	m_settings.setValue(getKey(AUTO_SAVE_PROJECT_KEY), enabled);
}

int ApplicationSettings::getTiffBwCompression() const
{
	return m_settings.value(getKey(TIFF_BW_COMPRESSION_KEY), COMPRESSION_CCITTFAX4).toInt();
}

void ApplicationSettings::setTiffBwCompression(int compression)
{
	m_settings.setValue(getKey(TIFF_BW_COMPRESSION_KEY), compression);
}

int ApplicationSettings::getTiffColorCompression() const
{
	return m_settings.value(getKey(TIFF_COLOR_COMPRESSION_KEY), COMPRESSION_LZW).toInt();
}

void ApplicationSettings::setTiffColorCompression(int compression)
{
	m_settings.setValue(getKey(TIFF_COLOR_COMPRESSION_KEY), compression);
}

bool ApplicationSettings::isBlackOnWhiteDetectionEnabled() const
{
	return m_settings.value(getKey(BLACK_ON_WHITE_DETECTION_KEY), true).toBool();
}

void ApplicationSettings::setBlackOnWhiteDetectionEnabled(bool enabled)
{
	m_settings.setValue(getKey(BLACK_ON_WHITE_DETECTION_KEY), enabled);
}

bool ApplicationSettings::isBlackOnWhiteDetectionOutputEnabled() const
{
	return m_settings.value(getKey(BLACK_ON_WHITE_DETECTION_OUTPUT_KEY), true).toBool();
}

void ApplicationSettings::setBlackOnWhiteDetectionOutputEnabled(bool enabled)
{
	m_settings.setValue(getKey(BLACK_ON_WHITE_DETECTION_OUTPUT_KEY), enabled);
}

bool ApplicationSettings::isHighlightDeviationEnabled() const
{
	return m_settings.value(getKey(HIGHLIGHT_DEVIATION_KEY), true).toBool();
}

void ApplicationSettings::setHighlightDeviationEnabled(bool enabled)
{
	m_settings.setValue(getKey(HIGHLIGHT_DEVIATION_KEY), enabled);
}

double ApplicationSettings::getDeskewDeviationCoef() const
{
	return m_settings.value(getKey(DESKEW_DEVIATION_COEF_KEY), 1.5).toDouble();
}

void ApplicationSettings::setDeskewDeviationCoef(double value)
{
	m_settings.setValue(getKey(DESKEW_DEVIATION_COEF_KEY), value);
}

double ApplicationSettings::getDeskewDeviationThreshold() const
{
	return m_settings.value(getKey(DESKEW_DEVIATION_THRESHOLD_KEY), 1.0).toDouble();
}

void ApplicationSettings::setDeskewDeviationThreshold(double value)
{
	m_settings.setValue(getKey(DESKEW_DEVIATION_THRESHOLD_KEY), value);
}

double ApplicationSettings::getSelectContentDeviationCoef() const
{
	return m_settings.value(getKey(SELECT_CONTENT_DEVIATION_COEF_KEY), 0.35).toDouble();
}

void ApplicationSettings::setSelectContentDeviationCoef(double value)
{
	m_settings.setValue(getKey(SELECT_CONTENT_DEVIATION_COEF_KEY), value);
}

double ApplicationSettings::getSelectContentDeviationThreshold() const
{
	return m_settings.value(getKey(SELECT_CONTENT_DEVIATION_THRESHOLD_KEY), 1.0).toDouble();
}

void ApplicationSettings::setSelectContentDeviationThreshold(double value)
{
	m_settings.setValue(getKey(SELECT_CONTENT_DEVIATION_THRESHOLD_KEY), value);
}

double ApplicationSettings::getMarginsDeviationCoef() const
{
	return m_settings.value(getKey(MARGINS_DEVIATION_COEF_KEY), 0.35).toDouble();
}

void ApplicationSettings::setMarginsDeviationCoef(double value)
{
	m_settings.setValue(getKey(MARGINS_DEVIATION_COEF_KEY), value);
}

double ApplicationSettings::getMarginsDeviationThreshold() const
{
	return m_settings.value(getKey(MARGINS_DEVIATION_THRESHOLD_KEY), 1.0).toDouble();
}

void ApplicationSettings::setMarginsDeviationThreshold(double value)
{
	m_settings.setValue(getKey(MARGINS_DEVIATION_THRESHOLD_KEY), value);
}

QSize ApplicationSettings::getThumbnailQuality() const
{
	return m_settings.value(getKey(THUMBNAIL_QUALITY_KEY), QSize(200, 200)).toSize();
}

void ApplicationSettings::setThumbnailQuality(QSize const& quality)
{
	m_settings.setValue(getKey(THUMBNAIL_QUALITY_KEY), quality);
}

QSizeF ApplicationSettings::getMaxLogicalThumbnailSize() const
{
	return m_settings.value(getKey(MAX_LOGICAL_THUMBNAIL_SIZE_KEY), QSizeF(250, 160)).toSizeF();
}

void ApplicationSettings::setMaxLogicalThumbnailSize(QSizeF const& size)
{
	m_settings.setValue(getKey(MAX_LOGICAL_THUMBNAIL_SIZE_KEY), size);
}

bool ApplicationSettings::isSingleColumnThumbnailDisplayEnabled() const
{
	return m_settings.value(getKey(SINGLE_COLUMN_THUMBNAIL_DISPLAY_KEY), false).toBool();
}

void ApplicationSettings::setSingleColumnThumbnailDisplayEnabled(bool enabled)
{
	m_settings.setValue(getKey(SINGLE_COLUMN_THUMBNAIL_DISPLAY_KEY), enabled);
}

QString ApplicationSettings::getLanguage() const
{
	return m_settings.value(getKey(LANGUAGE_KEY), QLocale::system().name()).toString();
}

void ApplicationSettings::setLanguage(QString const& language)
{
	m_settings.setValue(getKey(LANGUAGE_KEY), language);
}

QString ApplicationSettings::getUnits() const
{
	return m_settings.value(getKey(UNITS_KEY), QStringLiteral("mm")).toString();
}

void ApplicationSettings::setUnits(QString const& units)
{
	m_settings.setValue(getKey(UNITS_KEY), units);
}

QString ApplicationSettings::getCurrentProfile() const
{
	return m_settings.value(getKey(CURRENT_PROFILE_KEY), QStringLiteral("Default")).toString();
}

void ApplicationSettings::setCurrentProfile(QString const& profile)
{
	m_settings.setValue(getKey(CURRENT_PROFILE_KEY), profile);
}

bool ApplicationSettings::isCancelingSelectionQuestionEnabled() const
{
	return m_settings.value(getKey(SHOW_CANCELING_SELECTION_QUESTION_KEY), true).toBool();
}

void ApplicationSettings::setCancelingSelectionQuestionEnabled(bool enabled)
{
	m_settings.setValue(getKey(SHOW_CANCELING_SELECTION_QUESTION_KEY), enabled);
}

int ApplicationSettings::getBatchThreadCount() const
{
	return m_settings.value(getKey(BATCH_THREAD_COUNT_KEY), 0).toInt(); // 0 = auto (use idealThreadCount)
}

void ApplicationSettings::setBatchThreadCount(int count)
{
	m_settings.setValue(getKey(BATCH_THREAD_COUNT_KEY), count);
}
