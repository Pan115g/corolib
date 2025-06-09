/*
 * CRtcAdapter.h
 *
 *  Created on: Jun 6, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CRTCADAPTER_H_
#define SRC_COROLIB_CRTCADAPTER_H_

#include "stm32f4xx_hal.h"

namespace corolib {

class CRtcAdapter {
public:
    CRtcAdapter(RTC_TypeDef *rtcAddress);
    ~CRtcAdapter();

    bool setDateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t weekDay, uint8_t hour, uint8_t minute, uint8_t second);

    bool getDateTime(uint8_t &year, uint8_t &month, uint8_t &day, uint8_t &weekDay, uint8_t &hour, uint8_t &minute, uint8_t &second);
private:
    RTC_HandleTypeDef mRtc;
    bool mInitialized {false};
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CRTCADAPTER_H_ */
