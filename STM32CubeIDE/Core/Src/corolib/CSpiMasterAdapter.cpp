/*
 * CSpiMasterAdapter.cpp
 *
 *  Created on: Jun 5, 2025
 *      Author: pan.li
 */

#include <CSpiMasterAdapter.h>

namespace corolib {

CSpiMasterAdapter::CSpiMasterAdapter(SPI_TypeDef * spiAddress) {
    mSpi.Instance = spiAddress;
    mSpi.Init.Mode = SPI_MODE_MASTER;
    mSpi.Init.Direction = SPI_DIRECTION_2LINES;
    mSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    mSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    mSpi.Init.CLKPhase = SPI_PHASE_2EDGE;
    mSpi.Init.NSS = SPI_NSS_HARD_OUTPUT;
    mSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    mSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    mSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    mSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    mSpi.Init.CRCPolynomial = 10;
    mInitialized = HAL_SPI_Init(&mSpi) == HAL_OK;
}

CSpiMasterAdapter::~CSpiMasterAdapter() {

}

bool CSpiMasterAdapter::startTransfer(const std::span<uint8_t> txData, std::span<uint8_t> rxData)
{
    return mInitialized && HAL_SPI_TransmitReceive_DMA(&mSpi, (uint8_t*)txData.data(), (uint8_t*)rxData.data(),
            txData.size()) == HAL_OK;
}

} /* namespace corolib */
