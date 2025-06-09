/*
 * CSpiMasterAdapter.h
 *
 *  Created on: Jun 5, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CSPIMASTERADAPTER_H_
#define SRC_COROLIB_CSPIMASTERADAPTER_H_

#include "stm32f4xx_hal.h"
#include <span>

namespace corolib {

class CSpiMasterAdapter {
public:
    CSpiMasterAdapter(SPI_TypeDef * spiAddress);
    ~CSpiMasterAdapter();

    bool startTransfer(const std::span<uint8_t> txData, std::span<uint8_t> rxData);
private:
    SPI_HandleTypeDef mSpi;
    bool mInitialized {false};
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CSPIMASTERADAPTER_H_ */
