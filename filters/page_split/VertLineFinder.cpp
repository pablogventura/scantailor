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

#include "VertLineFinder.h"
#include "ImageTransformation.h"
#include "Dpi.h"
#include "DebugImages.h"
#include "imageproc/Transform.h"
#include "imageproc/GrayImage.h"
#include "imageproc/Grayscale.h"
#include "imageproc/GrayRasterOp.h"
#include "imageproc/Morphology.h"
#include "imageproc/MorphGradientDetect.h"
#include "imageproc/HoughLineDetector.h"
#include "imageproc/Constants.h"
#ifndef Q_MOC_RUN
#endif
#include <QLineF>
#include <QSizeF>
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QTransform>
#include <Qt>
#include <QDebug>
#include <list>
#include <algorithm>
#include <math.h>

namespace page_split
{

using namespace imageproc;

static double lineCenterX(QLineF const& line)
{
	return 0.5 * (line.p1().x() + line.p2().x());
}

std::vector<QLineF>
VertLineFinder::findLinesAtDpi(
	QImage const& image, ImageTransformation const& xform,
	int const dpi, int const max_lines,
	GrayImage* gray_downscaled, QTransform* out_to_downscaled,
	DebugImages* dbg)
{
	ImageTransformation xform_dpi(xform);
	xform_dpi.preScaleToDpi(Dpi(dpi, dpi));
	
	QRect target_rect(xform_dpi.resultingRect().toRect());
	if (target_rect.isEmpty()) {
		target_rect.setWidth(1);
		target_rect.setHeight(1);
	}

	GrayImage gray(
		transformToGray(
			image, xform_dpi.transform(), target_rect,
			OutsidePixels::assumeWeakColor(Qt::black), QSizeF(5.0, 5.0)
		)
	);
	if (dbg && dpi == 100) {
		dbg->add(gray, "gray100");
	}
	
	if (gray_downscaled && dpi == 100) {
		*gray_downscaled = gray;
	}
	if (out_to_downscaled && dpi == 100) {
		*out_to_downscaled = xform.transformBack() * xform_dpi.transform();
	}

	// Downweight spine shadow so the dark central band doesn't dominate Hough
	gray = maskSpineShadow(gray);
	GrayImage preprocessed(gray);
	gray = GrayImage();

	GrayImage h_gradient(erodeGray(preprocessed, QSize(11, 1), 0x00));
	GrayImage v_gradient(erodeGray(preprocessed, QSize(1, 11), 0x00));
	if (!dbg) {
		preprocessed = GrayImage();
	}
	
	grayRasterOp<GRopClippedSubtract<GRopDst, GRopSrc> >(h_gradient, v_gradient);
	v_gradient = GrayImage();
	if (dbg && dpi == 100) {
		dbg->add(h_gradient, "vert_raster_lines");
	}

	GrayImage const raster_lines(closeGray(h_gradient, QSize(1, 19), 0x00));
	h_gradient = GrayImage();
	if (dbg && dpi == 100) {
		dbg->add(raster_lines, "short_segments_removed");
	}
	
	double const line_thickness = 5.0;
	double const max_angle = 10.0; // degrees (slightly wider for tilted folds)
	double const angle_step = 0.25;
	int const angle_steps_to_max = (int)(max_angle / angle_step);
	int const total_angle_steps = angle_steps_to_max * 2 + 1;
	double const min_angle = -angle_steps_to_max * angle_step;
	HoughLineDetector line_detector(
		raster_lines.size(), line_thickness,
		min_angle, angle_step, total_angle_steps
	);
	
	unsigned weight_table[256];
	buildWeightTable(weight_table);

	double const margin_mm = 3.5;
	int const margin = (int)floor(0.5 + margin_mm * constants::MM2INCH * dpi);
	int const x_limit = raster_lines.width() - margin;
	int const height = raster_lines.height();
	uint8_t const* line_ptr = raster_lines.data();
	int const stride = raster_lines.stride();
	for (int y = 0; y < height; ++y, line_ptr += stride) {
		for (int x = margin; x < x_limit; ++x) {
			unsigned const val = line_ptr[x];
			if (val > 1) {
				line_detector.process(x, y, weight_table[val]);
			}
		}
	}
	
	unsigned const min_quality = (unsigned)(height * line_thickness * 1.8) + 1;
	
	if (dbg && dpi == 100) {
		dbg->add(line_detector.visualizeHoughSpace(min_quality), "hough_space");
	}
	
	std::vector<HoughLine> const hough_lines(line_detector.findLines(min_quality));
	
	typedef std::list<LineGroup> LineGroups;
	LineGroups line_groups;
	for (HoughLine const& hough_line : hough_lines) {
		QualityLine const new_line(
			hough_line.pointAtY(0.0),
			hough_line.pointAtY(height),
			hough_line.quality()
		);
		LineGroup* home_group = 0;
		LineGroups::iterator it(line_groups.begin());
		LineGroups::iterator const end(line_groups.end());
		while (it != end) {
			LineGroup& group = *it;
			if (group.belongsHere(new_line)) {
				if (home_group) {
					home_group->merge(group);
					line_groups.erase(it++);
					continue;
				} else {
					group.add(new_line);
					home_group = &group;
				}
			}
			++it;
		}
		if (!home_group) {
			line_groups.push_back(LineGroup(new_line));
		}
	}
	
	std::vector<QLineF> lines;
	for (LineGroup const& group : line_groups) {
		lines.push_back(group.leader().toQLine());
		if ((int)lines.size() == max_lines) {
			break;
		}
	}
	
	if (dbg && dpi == 100 && !preprocessed.isNull()) {
		QImage visual(
			preprocessed.toQImage().convertToFormat(
				QImage::Format_ARGB32_Premultiplied
			)
		);
		QPainter painter(&visual);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(QPen(QColor(0xff, 0x00, 0x00, 0x80), 3.0));
		for (QLineF const& ln : lines) {
			painter.drawLine(ln);
		}
		dbg->add(visual, "vector_lines");
	}
	
	return lines;
}

GrayImage
VertLineFinder::maskSpineShadow(GrayImage const& src)
{
	int const w = src.width();
	int const h = src.height();
	if (w < 20 || h < 10) {
		return src;
	}
	std::vector<double> col_mean(w, 0.0);
	uint8_t const* line = src.data();
	int const stride = src.stride();
	for (int y = 0; y < h; ++y, line += stride) {
		for (int x = 0; x < w; ++x) {
			col_mean[x] += line[x];
		}
	}
	double global = 0;
	for (int x = 0; x < w; ++x) {
		col_mean[x] /= h;
		global += col_mean[x];
	}
	global /= w;
	double const threshold = 0.5 * global;
	int const band_lo = w / 4;
	int const band_hi = 3 * w / 4;
	GrayImage dst(src);
	uint8_t* dst_line = dst.data();
	int const dst_stride = dst.stride();
	for (int x = band_lo; x < band_hi; ++x) {
		if (col_mean[x] >= threshold) {
			continue;
		}
		for (int y = 0; y < h; ++y) {
			dst_line[y * dst_stride + x] = 128;
		}
	}
	return dst;
}

std::vector<QLineF>
VertLineFinder::findLines(
	QImage const& image, ImageTransformation const& xform,
	int const max_lines, DebugImages* dbg,
	GrayImage* gray_downscaled, QTransform* out_to_downscaled)
{
	QRectF const orig_rect(xform.transform().mapRect(image.rect()));
	double const orig_width = orig_rect.width();
	double const cluster_threshold = 0.02 * orig_width;
	
	int const dpis[] = { 72, 100, 150 };
	std::vector<QLineF> all_lines;
	for (int i = 0; i < 3; ++i) {
		int const dpi = dpis[i];
		ImageTransformation xform_dpi(xform);
		xform_dpi.preScaleToDpi(Dpi(dpi, dpi));
		QTransform const undo(xform_dpi.transformBack() * xform.transform());
		std::vector<QLineF> lines = findLinesAtDpi(
			image, xform_dpi, dpi, max_lines * 2,
			(dpi == 100) ? gray_downscaled : 0,
			(dpi == 100) ? out_to_downscaled : 0,
			dbg
		);
		for (QLineF& line : lines) {
			line = undo.map(line);
			all_lines.push_back(line);
		}
	}
	
	if (all_lines.empty()) {
		return all_lines;
	}
	
	std::sort(all_lines.begin(), all_lines.end(),
		[](QLineF const& a, QLineF const& b) { return lineCenterX(a) < lineCenterX(b); });
	
	std::vector<QLineF> merged;
	double prev_center = lineCenterX(all_lines.front());
	QLineF rep = all_lines.front();
	for (QLineF const& line : all_lines) {
		double const cx = lineCenterX(line);
		if (cx - prev_center <= cluster_threshold) {
			double const image_center = orig_rect.center().x();
			if (std::fabs(cx - image_center) < std::fabs(lineCenterX(rep) - image_center)) {
				rep = line;
			}
			continue;
		}
		merged.push_back(rep);
		rep = line;
		prev_center = cx;
	}
	merged.push_back(rep);
	
	if ((int)merged.size() > max_lines) {
		merged.resize(max_lines);
	}
	return merged;
}

GrayImage
VertLineFinder::removeDarkVertBorders(GrayImage const& src)
{
	GrayImage dst(src);
	
	selectVertBorders(dst);
	grayRasterOp<GRopInvert<GRopClippedSubtract<GRopDst, GRopSrc> > >(dst, src);
	
	return dst;
}

void
VertLineFinder::selectVertBorders(GrayImage& image)
{
	int const w = image.width();
	int const h = image.height();
	
	unsigned char* image_line = image.data();
	int const image_stride = image.stride();
	
	std::vector<unsigned char> tmp_line(w, 0x00);
	
	for (int y = 0; y < h; ++y, image_line += image_stride) {
		// Left to right.
		unsigned char prev_pixel = 0x00; // Black vertical border.
		for (int x = 0; x < w; ++x) {
			prev_pixel = std::max(image_line[x], prev_pixel);
			tmp_line[x] = prev_pixel;
		}
		
		// Right to left
		prev_pixel = 0x00; // Black vertical border.
		for (int x = w - 1; x >= 0; --x) {
			prev_pixel = std::max(
				image_line[x],
				std::min(prev_pixel, tmp_line[x])
			);
			image_line[x] = prev_pixel;
		}
	}
}

void
VertLineFinder::buildWeightTable(unsigned weight_table[])
{
	int gray_level = 0;
	unsigned weight = 2;
	int segment = 2;
	int prev_segment = 1;
	
	while (gray_level < 256) {
		int const limit = std::min(256, gray_level + segment);
		for (; gray_level < limit; ++gray_level) {
			weight_table[gray_level] = weight;
		}
		++weight;
		segment += prev_segment;
		prev_segment = segment;
	}
}


/*======================= VertLineFinder::QualityLine =======================*/

VertLineFinder::QualityLine::QualityLine(
	QPointF const& top, QPointF const& bottom, unsigned const quality)
:	m_quality(quality)
{
	if (top.x() < bottom.x()) {
		m_left = top;
		m_right = bottom;
	} else {
		m_left = bottom;
		m_right = top;
	}
}

QLineF
VertLineFinder::QualityLine::toQLine() const
{
	return QLineF(m_left, m_right);
}


/*======================= VertLineFinder::LineGroup ========================*/

VertLineFinder::LineGroup::LineGroup(QualityLine const& line)
:	m_leader(line),
	m_left(line.left().x()),
	m_right(line.right().x())
{
}

bool
VertLineFinder::LineGroup::belongsHere(QualityLine const& line) const
{
	if (m_left > line.right().x()) {
		return false;
	} else if (m_right < line.left().x()) {
		return false;
	} else {
		return true;
	}
}

void
VertLineFinder::LineGroup::add(QualityLine const& line)
{
	m_left = std::min(qreal(m_left), line.left().x());
	m_right = std::max(qreal(m_right), line.right().x());
	if (line.quality() > m_leader.quality()) {
		m_leader = line;
	}
}

void
VertLineFinder::LineGroup::merge(LineGroup const& other)
{
	m_left = std::min(m_left, other.m_left);
	m_right = std::max(m_right, other.m_right);
	if (other.leader().quality() > m_leader.quality()) {
		m_leader = other.leader();
	}
}

} // namespace page_split
