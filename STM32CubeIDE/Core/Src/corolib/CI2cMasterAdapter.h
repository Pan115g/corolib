/*
 * CI2CMasterAdapter.h
 *
 *  Created on: Jun 12, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CI2CMASTERADAPTER_H_
#define SRC_COROLIB_CI2CMASTERADAPTER_H_

#include "stm32f4xx_hal.h"
#include <span>

namespace corolib {

class CI2cMasterAdapter {
public:
    CI2cMasterAdapter(I2C_TypeDef *i2cAdr, uint8_t deviceAdr);
    ~CI2cMasterAdapter();

    void setSlaveAddress(uint8_t slaveAdr) {mSlaveAdr = slaveAdr;};
    bool write(const std::span<uint8_t> buffer);
    bool read(std::span<uint8_t> buffer);
    bool writeFirstFrame(const std::span<uint8_t> buffer);
    bool writeRepeatedStart(const std::span<uint8_t> buffer);
    bool writeLastFrame(const std::span<uint8_t> buffer);
    bool readFirstFrame(std::span<uint8_t> buffer);
    bool readRepeatedStart(std::span<uint8_t> buffer);
    bool readLastFrame(std::span<uint8_t> buffer);

    void irqHandler();
private:
    I2C_HandleTypeDef mI2c;
    bool mInitialized {false};
    uint8_t mSlaveAdr {0U};
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CI2CMASTERADAPTER_H_ */
