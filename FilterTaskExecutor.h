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

#ifndef FILTERTASKEXECUTOR_H_
#define FILTERTASKEXECUTOR_H_

#include "NonCopyable.h"
#include "BackgroundTask.h"
#include "FilterResult.h"
#include <QObject>
#include <QPointer>

class FilterTaskExecutor : public QObject
{
	Q_OBJECT
	DECLARE_NON_COPYABLE(FilterTaskExecutor)
public:
	explicit FilterTaskExecutor(QObject* parent = 0);

	/**
	 * \brief Submit a task for execution in a background thread from the pool.
	 *
	 * The task will run in one of the QThreadPool threads. When done,
	 * taskResult() is emitted from the main thread.
	 */
	void submitTask(BackgroundTaskPtr const& task);

	/**
	 * \brief Maximum number of tasks that can run in parallel.
	 */
	static int maxParallelTasks();
signals:
	void taskResult(BackgroundTaskPtr const& task, FilterResultPtr const& result);
private:
	class TaskResultEvent;
	class FilterTaskRunnable;

	void customEvent(QEvent* event) override;
};

#endif
