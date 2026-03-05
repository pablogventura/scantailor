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

#include "FilterTaskExecutor.h"
#include "OutOfMemoryHandler.h"
#include <QCoreApplication>
#include <QEvent>
#include <QThread>
#include <QThreadPool>
#include <Qt>

class FilterTaskExecutor::TaskResultEvent : public QEvent
{
public:
	static QEvent::Type const eventType = static_cast<QEvent::Type>(QEvent::User + 2);

	TaskResultEvent(BackgroundTaskPtr const& task, FilterResultPtr const& result)
		: QEvent(static_cast<QEvent::Type>(eventType))
		, m_task(task)
		, m_result(result)
	{
	}

	BackgroundTaskPtr const& task() const { return m_task; }
	FilterResultPtr const& result() const { return m_result; }
private:
	BackgroundTaskPtr m_task;
	FilterResultPtr m_result;
};

class FilterTaskExecutor::FilterTaskRunnable : public QRunnable
{
public:
	FilterTaskRunnable(
		BackgroundTaskPtr const& task,
		QPointer<FilterTaskExecutor> executor)
		: m_task(task)
		, m_executor(executor)
	{
		setAutoDelete(true);
	}

	void run() override
	{
		FilterResultPtr result;
		try {
			if (!m_task->isCancelled()) {
				result = (*m_task)();
			}
		} catch (std::bad_alloc const&) {
			OutOfMemoryHandler::instance().handleOutOfMemorySituation();
		} catch (...) {
			// Other exceptions: result stays null, we still deliver so UI can continue
		}
		// Always post so MainWindow can call processingFinished() and update the queue
		if (m_executor) {
			QCoreApplication::postEvent(
				m_executor,
				new TaskResultEvent(m_task, result),
				Qt::HighEventPriority
			);
		}
	}
private:
	BackgroundTaskPtr m_task;
	QPointer<FilterTaskExecutor> m_executor;
};


FilterTaskExecutor::FilterTaskExecutor(QObject* parent)
	: QObject(parent)
{
}

void
FilterTaskExecutor::submitTask(BackgroundTaskPtr const& task)
{
	if (!task) {
		return;
	}
	QThreadPool::globalInstance()->start(
		new FilterTaskRunnable(task, QPointer<FilterTaskExecutor>(this))
	);
}

int
FilterTaskExecutor::maxParallelTasks()
{
	return QThreadPool::globalInstance()->maxThreadCount();
}

void
FilterTaskExecutor::customEvent(QEvent* event)
{
	if (event->type() == TaskResultEvent::eventType) {
		TaskResultEvent* ev = static_cast<TaskResultEvent*>(event);
		emit taskResult(ev->task(), ev->result());
		return;
	}
	QObject::customEvent(event);
}
