/*
 * CliWorker.cpp
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */

#include "CliWorker.h"
#include "CLogger.h"

using namespace cm;
using namespace cli;

// ------------------------------------------------
CliWorker::CliWorker(int index)
: Thread("CliWorkerThread"), m_index(index), m_task(0), m_executingTask(false)
{
    this->start();
}

// ------------------------------------------------
CliWorker::~CliWorker() {
    
}

// ------------------------------------------------
bool CliWorker::isIdle() {
    return (!m_executingTask);
}

// ------------------------------------------------
void CliWorker::setTask(Task* theTask) {
    if (!m_executingTask) {
        if (theTask != 0) {
            LOG_DBG(CLI_LOGGER_NAME, "[%s], set new task\n", __func__);
            m_lock.lock();
            m_task = theTask;
            m_lock.unlock();
            m_event.set();
        } else {
            LOG_ERROR(CLI_LOGGER_NAME, "[%s], task is null\n", __func__);
        }
    } else {
        LOG_ERROR(CLI_LOGGER_NAME, "[%s], worker is executing task, index = %d\n", __func__, m_index);
        if (theTask != 0) {
            delete theTask;
        }
    }
}

// ------------------------------------------------
unsigned long CliWorker::run() {
    while (true) {
        m_event.wait();

        LOG_DBG(CLI_LOGGER_NAME, "[%s], executing task begin\n", __func__);
        m_executingTask = true;

        m_lock.lock();
        m_task->execute();
        delete m_task;
        m_task = 0;
        m_lock.unlock();

        LOG_DBG(CLI_LOGGER_NAME, "[%s], executing task end\n", __func__);

        m_executingTask = false;
    }

    return 0;
}
