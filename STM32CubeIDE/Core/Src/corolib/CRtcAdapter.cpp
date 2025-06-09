/*
 * CRtcAdapter.cpp
 *
 *  Created on: Jun 6, 2025
 *      Author: pan.li
 */

#include <CRtcAdapter.h>

namespace corolib {

CRtcAdapter::CRtcAdapter(RTC_TypeDef *rtcAddress) {
    mRtc.Instance = rtcAddress;
    mRtc.Init.HourFormat = RTC_HOURFORMAT_24;
    mRtc.Init.AsynchPrediv = 127;
    mRtc.Init.SynchPrediv = 255;
    mRtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    mRtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    mRtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    mInitialized = HAL_RTC_Init(&mRtc) == HAL_OK;

}

CRtcAdapter::~CRtcAdapter() {
    // TODO Auto-generated destructor stub
}

bool CRtcAdapter::setDateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t weekDay,
        uint8_t hour, uint8_t minute, uint8_t second)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};

    time.Hours = hour;
    time.Minutes = minute;
    time.Seconds = second;
    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&mRtc, &time, RTC_FORMAT_BIN) != HAL_OK)
    {
        return false;
    }

    date.WeekDay = weekDay;
    date.Month = month;
    date.Date = day;
    date.Year = year;

    return HAL_RTC_SetDate(&mRtc, &date, RTC_FORMAT_BIN) == HAL_OK;
}

bool CRtcAdapter::getDateTime(uint8_t &year, uint8_t &month, uint8_t &day, uint8_t &weekDay, uint8_t &hour, uint8_t &minute, uint8_t &second)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};
    if (HAL_RTC_GetTime(&mRtc, &time, RTC_FORMAT_BIN) != HAL_OK ||
        HAL_RTC_GetDate(&mRtc, &date, RTC_FORMAT_BIN) != HAL_OK)
    {
        return false;
    }

    weekDay = date.WeekDay;
    month = date.Month;
    day = date.Date;
    year = date.Year;
    hour = time.Hours;
    minute = time.Minutes;
    second = time.Seconds;

    return true;
}

} /* namespace corolib */
