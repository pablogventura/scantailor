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

#include "PdfToImages.h"
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QProgressDialog>
#include <QApplication>
#include <QMessageBox>
#include <QCoreApplication>

bool
PdfToImages::isAvailable()
{
	QString const pdftoppm = QStandardPaths::findExecutable(QString::fromLatin1("pdftoppm"));
	return !pdftoppm.isEmpty();
}

bool
PdfToImages::convert(
	QString const& pdfPath,
	QString const& outputDir,
	QWidget* parent)
{
	QString const pdftoppm = QStandardPaths::findExecutable(QString::fromLatin1("pdftoppm"));
	if (pdftoppm.isEmpty()) {
		if (parent) {
			QMessageBox::warning(
				parent,
				QCoreApplication::translate("PdfToImages", "PDF conversion"),
				QCoreApplication::translate("PdfToImages",
					"pdftoppm was not found. Install poppler-utils to create projects from PDF.\n"
					"Example: sudo apt install poppler-utils")
			);
		}
		return false;
	}

	QDir dir(outputDir);
	if (!dir.exists() && !dir.mkpath(QString::fromLatin1("."))) {
		if (parent) {
			QMessageBox::warning(
				parent,
				QCoreApplication::translate("PdfToImages", "PDF conversion"),
				QCoreApplication::translate("PdfToImages", "Could not create directory: %1").arg(outputDir)
			);
		}
		return false;
	}

	// pdftoppm -png -r 300 input.pdf output_prefix  -> output_prefix-01.png, ...
	// Use "page" as prefix so we get page-01.png, page-02.png (good for ordering)
	QString const prefixPath = dir.absoluteFilePath(QString::fromLatin1("page"));
	QStringList args;
	args << QString::fromLatin1("-png")
	     << QString::fromLatin1("-r") << QString::fromLatin1("300")
	     << pdfPath
	     << QDir::toNativeSeparators(prefixPath);

	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	QProgressDialog* progress = nullptr;
	if (parent) {
		progress = new QProgressDialog(
			QCoreApplication::translate("PdfToImages", "Converting PDF to images (300 DPI, PNG)..."),
			QString(),
			0, 0,
			parent
		);
		progress->setWindowModality(Qt::WindowModal);
		progress->setMinimumDuration(0);
		progress->show();
		QApplication::processEvents();
	}

	process.start(pdftoppm, args);

	if (progress) {
		while (!process.waitForFinished(200)) {
			QApplication::processEvents();
			if (progress->wasCanceled()) {
				process.kill();
				process.waitForFinished(1000);
				progress->deleteLater();
				return false;
			}
		}
		progress->deleteLater();
	} else {
		process.waitForFinished(-1);
	}

	if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
		if (parent) {
			QString err(process.readAllStandardError());
			if (err.isEmpty()) err = process.readAllStandardOutput();
			QMessageBox::warning(
				parent,
				QCoreApplication::translate("PdfToImages", "PDF conversion failed"),
				QCoreApplication::translate("PdfToImages", "pdftoppm failed: %1").arg(err)
			);
		}
		return false;
	}

	// Check that we got at least one image
	QStringList names = dir.entryList(QStringList() << QString::fromLatin1("page-*.png"), QDir::Files, QDir::Name);
	if (names.isEmpty()) {
		if (parent) {
			QMessageBox::warning(
				parent,
				QCoreApplication::translate("PdfToImages", "PDF conversion"),
				QCoreApplication::translate("PdfToImages", "No pages were produced. The PDF may be empty or protected.")
			);
		}
		return false;
	}

	return true;
}
