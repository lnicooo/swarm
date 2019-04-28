#include "driver/spi_common.h"
#include "driver/spi_master.h"

// Defaults
#define SPIBUS_READ     (0x80)  /*!< addr | SPIBUS_READ  */
#define SPIBUS_WRITE    (0x7F)  /*!< addr & SPIBUS_WRITE */

#define MOSI 22
#define MISO 21
#define SCLK 23
#define CS  19
#define CLOCK_SPEED 1000000


spi_device_handle_t mpu9250_spi_init(void);
esp_err_t mpu9250_spi_read(spi_device_handle_t handle, uint8_t addr, const uint8_t *data, uint32_t length);
esp_err_t mpu9250_spi_write(spi_device_handle_t handle, uint8_t addr, const uint8_t *data, uint32_t length);
esp_err_t mpu9250_spi_set_addr(spi_device_handle_t* spi_handle);
esp_err_t mpu9250_spi_set_bus(spi_host_device_t spi_host);
