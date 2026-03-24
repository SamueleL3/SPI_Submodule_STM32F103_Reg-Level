#ifndef SPI_HPP
#define SPI_HPP



#include <stm32f1xx_hal.h>
#include <stdbool.h>
//---[ENUM SECTION]---
typedef enum {
    SPI_PHASE_1EDGE = 0, // CPHA = 0
    SPI_PHASE_2EDGE = 1  // CPHA = 1
} SPI_Phase;

typedef enum {
    SPI_POLARITY_LOW = 0,  // CPOL = 0
    SPI_POLARITY_HIGH = 1  // CPOL = 1
} SPI_Polarity;

typedef enum {
    SPI_DATA_MSB_FIRST = 0, // LSBFIRST = 0
    SPI_DATA_LSB_FIRST = 1  // LSBFIRST = 1
} SPI_DataOrder;

typedef enum {
    SPI_DATASIZE_8BIT = 0,  // DFF = 0
    SPI_DATASIZE_16BIT = 1  // DFF = 1
} SPI_DataSize;

typedef enum {
    SPI_BAUDRATE_DIV2   = 0x0,
    SPI_BAUDRATE_DIV4   = 0x1,
    SPI_BAUDRATE_DIV8   = 0x2,
    SPI_BAUDRATE_DIV16  = 0x3,
    SPI_BAUDRATE_DIV32  = 0x4,
    SPI_BAUDRATE_DIV64  = 0x5,
    SPI_BAUDRATE_DIV128 = 0x6,
    SPI_BAUDRATE_DIV256 = 0x7
} SPI_BaudRate;

typedef struct {
    SPI_BaudRate   baudRate;
    SPI_Polarity   polarity;
    SPI_Phase      phase;
    SPI_DataOrder  dataOrder;
    SPI_DataSize   dataSize;
} SPI_ConfigTypeDef;

typedef enum {
    SPI_STATE_READY = 0,
    SPI_STATE_BUSY_TX,
    SPI_STATE_BUSY_RX,
    SPI_STATE_BUSY_TX_RX
} SPI_State;

extern volatile SPI_State spi1_state;

//---[METHODS DECLARATION]---
void SPI1_GPIOConfigDefault();
void SPI2_GPIOConfigDefault();
void SPI_Receive(uint8_t *data, int size);
void SPI_Transmit(uint8_t *data, int size);
void SPI_TransmitReceive();
void CS_Enable();
void CS_Disable();
void SPI1_PeripheralConfigFD(const SPI_ConfigTypeDef* config);
void SPI1_PeripheralEnable();
void SPI1_PeripheralDisable();
void SPI2_PeripheralEnable();
void SPI2_PeripheralDisable();
// --- API GENERICHE DMA ---
void SPI1_DMA_Init();
void SPI1_DMA_Transmit(uint8_t* tx_buf, uint16_t size);
void SPI1_DMA_Receive(uint8_t* rx_buf, uint16_t size);
void SPI1_DMA_TransmitReceive(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t size);

bool SPI1_DMA_IsReady();

#endif /* SPI_HPP */
