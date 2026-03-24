# STM32F103 Register-Level SPI Submodule 🚀

This submodule provides an optimized, register-level SPI implementation for STM32F103 microcontrollers. Developed in C++, it allows you to interface with SPI peripherals ensuring maximum performance, while maintaining the simplicity typical of a high-level library.

## ⚡ Advantages
* **Zero Latency & No Overhead:** By directly accessing the hardware, the code bypasses the redundant checks of the standard ST HAL functions.
* **Optimized Resources:** Massive savings in terms of RAM memory and Flash space footprint.
* **Developer-Centric:** Despite being a low-level driver, it is incredibly easy to use thanks to intuitive and well-structured C++ APIs.

## ⚙️ Main Features
* **Simple Configuration:** Based on clear `struct`s and strongly typed `enum`s (e.g., `SPI_Polarity`, `SPI_BaudRate`, `SPI_DataOrder`) to prevent setup errors.
* **Multiple Profiles:** Ability to easily define multiple configurations in `SPIConfig.hpp` and simply pass them as a pointer to the setup function.
* **Integrated DMA Support:** Dedicated APIs to leverage DMA channels (`SPI1_DMA_Init`, `SPI1_DMA_Transmit`, 'SPI_DMA_TransmitReceive', `SPI1_DMA_Receive`) for efficient asynchronous transfers.
* **Fast CS Control:** Dedicated ready-to-use `CS_Enable()` and `CS_Disable()` functions.

## 🛠️ Configuration
Setup management is centralized and clean. You can declare all your required configurations directly in the `SPIConfig.hpp` file.

```cpp
// Configuration example extracted from SPIConfig.hpp
const SPI_ConfigTypeDef spi1_default_config = {
    SPI_BAUDRATE_DIV16,   // .baudRate
    SPI_POLARITY_LOW,     // .polarity
    SPI_PHASE_1EDGE,      // .phase
    SPI_DATA_LSB_FIRST,   // .dataOrder
    SPI_DATASIZE_8BIT     // .dataSize
};```


## 💻 Usage Example
Thanks to the high-level APIs, sending data via DMA with the STM32F103 requires only a few lines of code:

```cpp
#include "main_cpp.hpp"
#include "SPIConfig.hpp"

uint8_t data[64] = {0x30}; // Test buffer

void mainLoop_cpp() {
    // 1. Default GPIO Pin Initialization
    SPI1_GPIOConfigDefault();
    
    // 2. DMA Setup
    SPI1_DMA_Init();
    
    // 3. Apply the configuration by passing its pointer
    SPI1_PeripheralConfigFD(&spi1_default_config);
    
    // 4. Enable the peripheral
    SPI1_PeripheralEnable();
    CS_Disable();

    while(true) {
        // Pull Chip Select low
        CS_Enable();
        
        // Start asynchronous DMA transmission
        SPI1_DMA_Transmit(data, 64);
        
        // Wait for the DMA transfer to complete (lightweight polling)
        while(!SPI1_DMA_IsReady()) {
            // Perform other operations in the meantime...
        }
        
        // Pull Chip Select high
        CS_Disable();

        HAL_Delay(1000);
    }
}
```

## 📥 How to import the submodule into your projects
1. Add this repository as a Git submodule inside your STM32 project (ideally cloning it into a convenient path like `Core/Submodules/SPI`).
2. In your compiler/IDE settings (e.g., *STM32CubeIDE*), go to **Include paths** and add the path to the `Inc` folder of this submodule, so the compiler can resolve the `#include` directives.
3. Make sure the source `.cpp` files are included in your build paths (Source location).
4. Include `"SPIConfig.hpp"` or `"SPI.hpp"` in your scripts and start writing high-performance code!
