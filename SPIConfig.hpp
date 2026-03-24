/*
* SPIConfig.hpp
*
*  Created on: Feb 27, 2026
*      Author: Samuele
*/
#include "SPI.hpp"
#ifndef INC_SPICONFIG_HPP_
#define INC_SPICONFIG_HPP_

#define SPI1_USE_DMA

//Here can be definied multiple SPI configs and then you can pass by pointer that structure to the SPI1_PeripheralConfigFD method eg. SPI1_PeripheralConfigFD(&spi1_test_config)
const SPI_ConfigTypeDef spi1_default_config = {
    SPI_BAUDRATE_DIV16,   // .baudRate
    SPI_POLARITY_LOW,     // .polarity
    SPI_PHASE_1EDGE,      // .phase
    SPI_DATA_LSB_FIRST,   // .dataOrder
    SPI_DATASIZE_8BIT     // .dataSize
};

#endif /* INC_SPICONFIG_HPP_ */
