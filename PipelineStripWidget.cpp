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

#include "PipelineStripWidget.h"
#include "StageSequence.h"
#include "AbstractFilter.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVariant>

PipelineStripWidget::PipelineStripWidget(QWidget* parent)
	: QWidget(parent)
	, m_selectedIndex(-1)
	, m_batchProcessingPossible(false)
	, m_batchProcessingInProgress(false)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(4, 4, 4, 4);
	layout->setSpacing(2);
}

void PipelineStripWidget::setStages(IntrusivePtr<StageSequence> const& stages)
{
	m_ptrStages = stages;
	rebuildButtons();
	if (m_ptrStages && m_ptrStages->count() > 0 && m_selectedIndex < 0) {
		setSelectedStage(0);
	}
}

void PipelineStripWidget::setSelectedStage(int index)
{
	if (m_selectedIndex == index) {
		return;
	}
	m_selectedIndex = index;
	setButtonChecked(index);
}

void PipelineStripWidget::setBatchProcessingPossible(bool possible)
{
	m_batchProcessingPossible = possible;
}

void PipelineStripWidget::setBatchProcessingInProgress(bool in_progress)
{
	m_batchProcessingInProgress = in_progress;
	setEnabled(!in_progress);
}

void PipelineStripWidget::onStageButtonClicked()
{
	QObject* sender = QObject::sender();
	for (int i = 0; i < m_stageButtons.size(); ++i) {
		if (m_stageButtons[i] == sender) {
			m_selectedIndex = i;
			setButtonChecked(i);
			emit stageSelected(i);
			return;
		}
	}
}

void PipelineStripWidget::rebuildButtons()
{
	for (QToolButton* btn : m_stageButtons) {
		btn->deleteLater();
	}
	m_stageButtons.clear();

	QLayout* layout = this->layout();
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(layout);
	if (!hLayout || !m_ptrStages) {
		return;
	}

	int const count = m_ptrStages->count();
	for (int i = 0; i < count; ++i) {
		if (i > 0) {
			QLabel* arrow = new QLabel(QString::fromUtf8("→"));
			arrow->setStyleSheet("color: #9CA3AF; font-size: 12px;");
			hLayout->addWidget(arrow);
		}
		QString name = m_ptrStages->filterAt(i)->getName();
		QToolButton* btn = new QToolButton(this);
		btn->setText(name);
		btn->setCheckable(true);
		btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
		btn->setProperty("stageIndex", QVariant(i));
		btn->setStyleSheet(
			"QToolButton { padding: 6px 12px; border: none; border-radius: 6px; "
			"color: #6B6B6B; background: transparent; } "
			"QToolButton:hover { background: #E8E8EA; color: #1A1A1A; } "
			"QToolButton:checked { background: #E0E7FF; color: #2563EB; font-weight: 500; } "
			"QToolButton:disabled { color: #9CA3AF; }"
		);
		connect(btn, &QToolButton::clicked, this, &PipelineStripWidget::onStageButtonClicked);
		m_stageButtons.append(btn);
		hLayout->addWidget(btn);
	}
	hLayout->addStretch();

	if (m_selectedIndex >= 0 && m_selectedIndex < m_stageButtons.size()) {
		setButtonChecked(m_selectedIndex);
	}
}

void PipelineStripWidget::setButtonChecked(int index)
{
	for (int i = 0; i < m_stageButtons.size(); ++i) {
		m_stageButtons[i]->setChecked(i == index);
	}
}
