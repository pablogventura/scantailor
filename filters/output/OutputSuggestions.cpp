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

#include "OutputSuggestions.h"
#include "imageproc/BinaryImage.h"
#include "imageproc/BinaryThreshold.h"
#include "imageproc/ConnCompEraser.h"
#include "imageproc/Connectivity.h"
#include "imageproc/Grayscale.h"
#include <QImage>
#include <QRect>
#include <QtGlobal>
#include <algorithm>

namespace output
{

ColorParams::ColorMode
OutputSuggestions::suggestColorMode(QImage const& img, QRect const& contentRect)
{
	if (img.isNull()) {
		return ColorParams::BLACK_AND_WHITE;
	}
	QRect const rect = contentRect.isEmpty() ? img.rect() : contentRect.intersected(img.rect());
	if (rect.isEmpty()) {
		return ColorParams::BLACK_AND_WHITE;
	}
	QImage::Format const fmt = img.format();
	if (fmt == QImage::Format_Grayscale8 || fmt == QImage::Format_Indexed8) {
		return ColorParams::BLACK_AND_WHITE;
	}
	if (fmt != QImage::Format_RGB32 && fmt != QImage::Format_ARGB32 &&
	    fmt != QImage::Format_RGB888 && fmt != QImage::Format_ARGB32_Premultiplied) {
		return ColorParams::BLACK_AND_WHITE;
	}
	int const step = 10;
	int colored = 0;
	int total = 0;
	int const chroma_threshold = 18;
	for (int y = rect.top(); y <= rect.bottom(); y += step) {
		for (int x = rect.left(); x <= rect.right(); x += step) {
			QRgb const c = img.pixel(x, y);
			int const r = qRed(c);
			int const g = qGreen(c);
			int const b = qBlue(c);
			int const lo = std::min({r, g, b});
			int const hi = std::max({r, g, b});
			if (hi - lo > chroma_threshold) {
				++colored;
			}
			++total;
		}
	}
	if (total == 0) {
		return ColorParams::BLACK_AND_WHITE;
	}
	double const ratio = static_cast<double>(colored) / total;
	return (ratio > 0.05) ? ColorParams::COLOR_GRAYSCALE : ColorParams::BLACK_AND_WHITE;
}

DespeckleLevel
OutputSuggestions::suggestDespeckleLevel(QImage const& img, QRect const& contentRect)
{
	if (img.isNull()) {
		return DESPECKLE_CAUTIOUS;
	}
	QRect rect = contentRect.isEmpty() ? img.rect() : contentRect.intersected(img.rect());
	if (rect.isEmpty()) {
		return DESPECKLE_CAUTIOUS;
	}
	QImage work = img.copy(rect);
	if (work.format() != QImage::Format_Grayscale8) {
		work = imageproc::toGrayscale(work);
	}
	int const max_side = 200;
	if (work.width() > max_side || work.height() > max_side) {
		work = work.scaled(
			max_side, max_side,
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation
		);
	}
	if (work.isNull()) {
		return DESPECKLE_CAUTIOUS;
	}
	imageproc::BinaryImage const bin(
		work, work.rect(),
		imageproc::BinaryThreshold(128)
	);
	if (bin.isNull()) {
		return DESPECKLE_CAUTIOUS;
	}
	int total_black = 0;
	int speckle_black = 0;
	int const max_speckle_area = 25;
	imageproc::ConnCompEraser eraser(bin, imageproc::CONN4);
	for (;;) {
		imageproc::ConnComp const cc(eraser.nextConnComp());
		if (cc.isNull()) {
			break;
		}
		int const area = cc.width() * cc.height();
		total_black += area;
		if (area >= 1 && area <= max_speckle_area) {
			speckle_black += area;
		}
	}
	if (total_black < 50) {
		return DESPECKLE_OFF;
	}
	double const ratio = static_cast<double>(speckle_black) / total_black;
	if (ratio > 0.18) {
		return DESPECKLE_AGGRESSIVE;
	}
	if (ratio > 0.07) {
		return DESPECKLE_NORMAL;
	}
	if (ratio > 0.025) {
		return DESPECKLE_CAUTIOUS;
	}
	return DESPECKLE_OFF;
}

} // namespace output
