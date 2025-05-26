/*
 * CTrace.h
 *
 *  Created on: May 22, 2025
 *      Author: pan.li
 */

#ifndef SRC_CTRACE_H_
#define SRC_CTRACE_H_

#include "cmsis_os.h"

#ifdef DEBUG

class CTrace
{
    public:
    CTrace();

    static void runThread(void*);

    private:
    osThreadId_t mThread;
};

#endif

#endif /* SRC_CTRACE_H_ */
