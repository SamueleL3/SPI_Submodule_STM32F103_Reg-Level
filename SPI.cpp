#include "SPI.hpp"
#include "SPIConfig.hpp"

volatile SPI_State spi1_state = SPI_STATE_READY;

void SPI1_GPIOConfigDefault()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Enable GPIOA clock

    GPIOA->CRL &= ~0xFFFF0000;   //Reset bits from 16 to 31
    GPIOA->CRL |= (0xB << 20);   // PA5 (SCK) AF output Push Pull
    GPIOA->CRL |= (0xB << 28);   // PA7 (MOSI) AF output Push Pull
    GPIOA->CRL |= (0x1 << 26);   // PA6 (MISO) Input mode (floating)
    GPIOA->CRL |= (0x3 << 16);   // PA4 used for CS, GPIO Output
}

void SPI2_GPIOConfigDefault()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    GPIOB->CRH &= ~0xFFFF0000;  // Reset bits from 16 to 31
    GPIOB->CRH |= (0x3 << 16);  // PB12: CS
    GPIOB->CRH |= (0xB << 20);  // PB13:(SCK) AF output Push Pull
    GPIOB->CRH |= (0x4 << 24);  // PB14: (MISO) Input mode (floating)
    GPIOB->CRH |= (0xB << 28);  // PB15: (MOSI) AF output Push Pull
}

void CS_Enable()
{
    GPIOA->BSRR |= (1<<4)<<16U;
}

void CS_Disable()
{
    GPIOA->BSRR |= (1<<4);
}

void SPI1_PeripheralEnable()
{
    SPI1->CR1 |= (0x1<<6);   // SPE=1, Peripheral enabled
}

void SPI1_PeripheralDisable()
{
    SPI1->CR1 &= ~(0x1<<6);   // SPE=0, Peripheral Disabled
}

void SPI1_PeripheralConfigFD(const SPI_ConfigTypeDef* config)
{
    SPI1_PeripheralDisable();
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;  // Enable SPI1 Clock
    SPI1_PeripheralDisable();

    SPI1->CR1 |= (config->baudRate<<3);
    SPI1->CR1 |= (config->polarity<<1);
    SPI1->CR1 |= config->phase;
    SPI1->CR1 |= (config->dataOrder<<7);
    SPI1->CR1 |= (config->dataSize<<11);

    SPI1->CR1 &= ~(1<<10);  // RXONLY = 0, full-duplex
    SPI1->CR1 |= (1<<2);    // MSTR=1, Master mode
    SPI1->CR1 &= ~(1<<11);  // DFF=0, 8 bit data
    // software slave management must be selected
    SPI1->CR1 |= (1<<9) | (1<<8);  // SSM = 1, SSI = 1

    SPI1->CR2 = 0;

    #ifdef SPI1_USE_DMA
    SPI1->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN; //Enable DMA both on TX and RX
    #endif
}

void SPI_Receive (uint8_t *data, int size)
{
    while (size)
    {
        SPI1->DR = 0;  // send dummy data
        while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set
        *data++ = (SPI1->DR);
        size--;
    }
}

void SPI_Transmit (uint8_t *data, int size)
{
    if (!(SPI1->CR1&(1<<6)))
    {
        CS_Enable();
    }
    int i=0;
    while (i<size)
    {
       while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set
       SPI1->DR = data[i];  // load the data into the Data Register
       i++;
    }

    while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set
    while (((SPI1->SR)&(1<<7))) {};   // wait for BSY bit to Reset

    //clear OVR Flag by reading the data, and status register
    volatile uint8_t temp = SPI1->DR;
    temp = SPI1->SR;
    (void)temp;
}

void SPI1_DMA_Init()
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    NVIC_EnableIRQ(DMA1_Channel2_IRQn); // Interrupt RX
    NVIC_EnableIRQ(DMA1_Channel3_IRQn); // Interrupt TX
}

bool SPI1_DMA_IsReady()
{
    if (spi1_state != SPI_STATE_READY)
    {
        return false;
    }

    while (!(SPI1->SR & (1<<1))) {} // Wait for buffer to be empty(TXE = 1)
    while (SPI1->SR & (1<<7)) {}    // Fix pin (BSY = 0)

    //return true for then calling CS_DISABLE
    return true;
}

void SPI1_DMA_TransmitReceive(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t size)
{
    if (spi1_state != SPI_STATE_READY) return;

    spi1_state = SPI_STATE_BUSY_TX_RX;

    DMA1_Channel3->CCR &= ~DMA_CCR_EN; // TX
    DMA1_Channel2->CCR &= ~DMA_CCR_EN; // RX
    DMA1->IFCR |= DMA_IFCR_CGIF3 | DMA_IFCR_CGIF2;

    // Config TX  (Channel 3)
    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CMAR = (uint32_t)tx_buf;
    DMA1_Channel3->CNDTR = size;
    DMA1_Channel3->CCR = DMA_CCR_DIR | DMA_CCR_MINC;

    // Config RX (Channel 2)
    DMA1_Channel2->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel2->CMAR = (uint32_t)rx_buf;
    DMA1_Channel2->CNDTR = size;
    DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_TCIE; // TCIE: enable interrupt!

    DMA1_Channel2->CCR |= DMA_CCR_EN;
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

void SPI1_DMA_Transmit(uint8_t* tx_buf, uint16_t size)
{
    if (spi1_state != SPI_STATE_READY) return;
    spi1_state = SPI_STATE_BUSY_TX;

    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR |= DMA_IFCR_CGIF3;

    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CMAR = (uint32_t)tx_buf;
    DMA1_Channel3->CNDTR = size;
    DMA1_Channel3->CCR = DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_TCIE;

    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

// ========================================================
// INTERRUPTS CALLBACKS
// ========================================================
extern "C"
{
    void DMA1_Channel2_IRQHandler(void)
    {
        if (DMA1->ISR & DMA_ISR_TCIF2)
        {
            DMA1->IFCR |= DMA_IFCR_CTCIF2; // Clean flag
            spi1_state = SPI_STATE_READY;  // Free SPIModule
        }
    }

    void DMA1_Channel3_IRQHandler(void)
    {
        if (DMA1->ISR & DMA_ISR_TCIF3)
        {
            DMA1->IFCR |= DMA_IFCR_CTCIF3;
            if (spi1_state == SPI_STATE_BUSY_TX) {
                spi1_state = SPI_STATE_READY; // Free SPIModule
            }
        }
    }
}
