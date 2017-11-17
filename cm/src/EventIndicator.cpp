/*
 * EventIndicator.cpp
 *
 *  Created on: Apr 22, 2016
 *      Author: z.j
 */

#include "EventIndicator.h"
#include "CLogger.h"

using namespace cm;

// --------------------------------------------
EventIndicator::EventIndicator(bool eventIsSet) 
: m_eventIsSet(eventIsSet)
{
    int result = pthread_mutex_init(&m_mutex, 0);
    if (result == 0) {
        result = pthread_cond_init(&m_condition, 0);
        if (result != 0) {
            LOG_ERROR(CM_LOGGER_NAME, "Fail to init pthread_cond_t"); 
            pthread_mutex_destroy(&m_mutex);
        }
    } else {
        LOG_ERROR(CM_LOGGER_NAME, "Fail to init pthread_mutex_t"); 
    }
}

// --------------------------------------------
EventIndicator::~EventIndicator() {
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_condition);    
}

// --------------------------------------------
void EventIndicator::wait() {
    LOG_DBG(CM_LOGGER_NAME, "EventIndicator::wait()");

    int result = pthread_mutex_lock(&m_mutex);
    if (result != 0) {
        LOG_ERROR(CM_LOGGER_NAME, "Fail to lock on mutex.");
        return;
    }

    while (!m_eventIsSet) {       
        result = pthread_cond_wait(&m_condition, &m_mutex);
        if (result != 0) {
            LOG_ERROR(CM_LOGGER_NAME, "Fail to wait on condition.");
            pthread_mutex_unlock(&m_mutex);
            return;
        }
    }

    m_eventIsSet = false;
    pthread_mutex_unlock(&m_mutex);
}

// --------------------------------------------
void EventIndicator::set() {
    LOG_DBG(CM_LOGGER_NAME, "EventIndicator::set()");

    int result = pthread_mutex_lock(&m_mutex);
    if (result != 0) {
        LOG_ERROR(CM_LOGGER_NAME, "Fail to lock on mutex.");
        return;
    }        

    m_eventIsSet = true; 
    result = pthread_cond_signal(&m_condition);
    if (result != 0) {
        LOG_ERROR(CM_LOGGER_NAME, "Fail to signal.");
    }

    pthread_mutex_unlock(&m_mutex);
}

// --------------------------------------------
void EventIndicator::reset() {
    LOG_DBG(CM_LOGGER_NAME, "EventIndicator::reset()");

    int result = pthread_mutex_lock(&m_mutex);
    if (result != 0) {
        LOG_ERROR(CM_LOGGER_NAME, "Fail to lock on mutex.");
        return;
    }        

    m_eventIsSet = false; 
    result = pthread_cond_signal(&m_condition);
    if (result != 0) {
        LOG_ERROR(CM_LOGGER_NAME, "Fail to signal.");
    }

    pthread_mutex_unlock(&m_mutex);
}
