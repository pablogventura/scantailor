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

#ifndef PDFTOIMAGES_H_
#define PDFTOIMAGES_H_

#include <QString>

class QWidget;

/**
 * Converts PDF pages to PNG images using pdftoppm (poppler-utils).
 * Resolution 300 DPI and PNG format are chosen as suitable for book scanning.
 * \return Path to the directory containing the generated images (page-01.png, ...),
 *         or empty string on failure or if pdftoppm is not available.
 */
class PdfToImages
{
public:
	/** Check if pdftoppm is available. */
	static bool isAvailable();

	/**
	 * Convert PDF to PNGs in outputDir. Creates outputDir if needed.
	 * \param pdfPath Full path to the PDF file.
	 * \param outputDir Directory where page-01.png, page-02.png, ... will be created.
	 * \param parent Optional parent for progress/cancel dialog.
	 * \return true on success, false on error or cancel.
	 */
	static bool convert(
		QString const& pdfPath,
		QString const& outputDir,
		QWidget* parent = nullptr
	);
};

#endif
