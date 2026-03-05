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

#ifndef PIPELINESTRIPWIDGET_H_
#define PIPELINESTRIPWIDGET_H_

#include "IntrusivePtr.h"
#include "StageSequence.h"
#include <QWidget>
#include <QVector>

class QToolButton;

/**
 * Horizontal strip of stage buttons (Split → Deskew → Margins → ...).
 * Replaces the vertical StageListView in the 2026 UI redesign.
 */
class PipelineStripWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PipelineStripWidget(QWidget* parent = nullptr);

	void setStages(IntrusivePtr<StageSequence> const& stages);

	/** Set the currently selected stage by index (0-based). */
	void setSelectedStage(int index);

	/** Enable or disable the "Process all" action in the UI. */
	void setBatchProcessingPossible(bool possible);

	void setBatchProcessingInProgress(bool in_progress);

signals:
	void stageSelected(int index);

private slots:
	void onStageButtonClicked();

private:
	void rebuildButtons();
	void setButtonChecked(int index);

	IntrusivePtr<StageSequence> m_ptrStages;
	QVector<QToolButton*> m_stageButtons;
	int m_selectedIndex;
	bool m_batchProcessingPossible;
	bool m_batchProcessingInProgress;
};

#endif
