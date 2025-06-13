/*
 * CI2cMasterAdapter.cpp
 *
 *  Created on: Jun 12, 2025
 *      Author: pan.li
 */

#include <CI2cMasterAdapter.h>

namespace corolib {

CI2cMasterAdapter::CI2cMasterAdapter(I2C_TypeDef *i2cAdr, uint8_t deviceAdr) {
    mI2c.Instance = i2cAdr;
    mI2c.Init.ClockSpeed = 100000;
    mI2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    mI2c.Init.OwnAddress1 = (deviceAdr << 1U);
    mI2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    mI2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    mI2c.Init.OwnAddress2 = 0;
    mI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    mI2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    mInitialized = (HAL_I2C_Init(&mI2c) == HAL_OK);

}

CI2cMasterAdapter::~CI2cMasterAdapter() {
    // TODO Auto-generated destructor stub
}

bool CI2cMasterAdapter::write(const std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Transmit_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size()) == HAL_OK;
}

bool CI2cMasterAdapter::read(std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Receive_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size()) == HAL_OK;
}

bool CI2cMasterAdapter::writeFirstFrame(const std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Seq_Transmit_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size(), I2C_FIRST_FRAME) == HAL_OK;
}

bool CI2cMasterAdapter::writeRepeatedStart(const std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Seq_Transmit_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size(), I2C_NEXT_FRAME) == HAL_OK;
}

bool CI2cMasterAdapter::writeLastFrame(const std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Seq_Transmit_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size(), I2C_LAST_FRAME) == HAL_OK;
}

bool CI2cMasterAdapter::readFirstFrame(std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Seq_Receive_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size(), I2C_FIRST_FRAME) == HAL_OK;
}

bool CI2cMasterAdapter::readRepeatedStart(std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Seq_Receive_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size(), I2C_NEXT_FRAME) == HAL_OK;
}

bool CI2cMasterAdapter::readLastFrame(std::span<uint8_t> buffer)
{
    return HAL_I2C_Master_Seq_Receive_IT(&mI2c, (mSlaveAdr << 1U), buffer.data(), buffer.size(), I2C_LAST_FRAME) == HAL_OK;
}

void CI2cMasterAdapter::irqHandler()
{
    HAL_I2C_EV_IRQHandler(&mI2c);
}
} /* namespace corolib */
