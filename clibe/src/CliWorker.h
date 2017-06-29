/*
 * CliWorker.h
 *
 *  Created on: June 14, 2017
 *      Author: j.zhou
 */

#ifndef CLI_WORKER_H
#define CLI_WORKER_H

#include "Thread.h"
#include "EventIndicator.h"
#include "Task.h"
#include "MutexLock.h"

namespace cli {

    class CliWorker : public cm::Thread {
    public:
        CliWorker(int index);
        virtual ~CliWorker();

        virtual unsigned long run();

        bool isIdle();
        void setTask(cm::Task* theTask);
        void setUserData(int userData);
        int getUserData() const;

    private:
        int m_index;
        cm::EventIndicator m_event;
        cm::MutexLock m_lock;
        cm::Task* m_task;
        volatile bool m_executingTask;

        int m_userData;
    };

    // --------------------------------------
    inline void CliWorker::setUserData(int userData) {
        m_userData = userData;
    }

    // --------------------------------------
    inline int CliWorker::getUserData() const {
        return m_userData;
    }

}

#endif
