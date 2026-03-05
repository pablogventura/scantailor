/*
	Scan Tailor - Interactive post-processing tool for scanned pages.
	Copyright (C)  Joseph Artsimovich <joseph_a@mail.ru>

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

#include "OpenGLSupport.h"
#include "config.h"
#include <QSettings>
#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#endif

bool
OpenGLSupport::supported()
{
#ifndef ENABLE_OPENGL
	return false;
#else
	QSurfaceFormat format;
	format.setSamples(4);
	format.setStencilBufferSize(8);
	format.setAlphaBufferSize(8);
	QOpenGLWidget widget;
	widget.setFormat(format);
	widget.show();
	format = widget.format();
	if (format.samples() < 1) {
		return false;
	}
	if (format.stencilBufferSize() < 1) {
		return false;
	}
	if (format.alphaBufferSize() < 1) {
		return false;
	}
	return true;
#endif
}
