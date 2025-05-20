/*
 * CDeviceCreator.h
 *
 *  Created on: May 9, 2025
 *      Author: pan.li
 */

#ifndef SRC_CDEVICECREATOR_H_
#define SRC_CDEVICECREATOR_H_

#include "CUartAdapter.h"
#include "CAdcAdapter.h"

class CDeviceCreator {
public:


    static CDeviceCreator& getInstance();

    corolib::CUartAdapter& getUart2() {return mUart2;};
    corolib::CAdcAdapter& getAdc1(){return mAdc1;}
private:
    CDeviceCreator();
    CDeviceCreator(const CDeviceCreator&) = delete;

    corolib::CUartAdapter mUart2;
    corolib::CAdcAdapter mAdc1;
};


#endif /* SRC_COROLIB_CDEVICECREATOR_H_ */
