#include <stdio.h>
#include <stdbool.h>
#include "mpu9250_spi.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

spi_host_device_t host;

esp_err_t
mpu9250_spi_set_bus(spi_host_device_t spi_host)
{
  
  spi_bus_config_t buscfg={
    .mosi_io_num = MOSI,
    .miso_io_num = MISO,
    .sclk_io_num = SCLK,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz=SPI_MAX_DMA_LEN
  };
  host = spi_host;
  //Initialize the SPI bus
  esp_err_t ret=spi_bus_initialize(host, &buscfg, 0);
  
  return ret;
}

esp_err_t
mpu9250_spi_set_addr(spi_device_handle_t* spi_handle)
{

  spi_device_interface_config_t devcfg={
    .command_bits = 0,
    .address_bits = 8,
    .dummy_bits = 0,
    .mode = 0,
    .duty_cycle_pos = 128,
    .cs_ena_pretrans = 0,
    .cs_ena_posttrans = 0,
    .clock_speed_hz = CLOCK_SPEED,  
    .spics_io_num = CS,
    .flags = 0,  // 0 
    .queue_size = 1,
    .pre_cb = NULL,
    .post_cb = NULL,
     
  };

  esp_err_t ret=spi_bus_add_device(host, &devcfg, spi_handle);

  return ret;
}

esp_err_t
mpu9250_spi_read(spi_device_handle_t handle, uint8_t addr, const uint8_t *data, uint32_t length)
{
  spi_transaction_t transaction;
  transaction.flags = 0;
  transaction.cmd = 0;
  transaction.addr = addr | SPIBUS_READ;
  transaction.length = length * 8;
  transaction.rxlength = length * 8;
  transaction.user = NULL;
  transaction.tx_buffer = NULL;
  transaction.rx_buffer = data;
  esp_err_t ret = spi_device_transmit(handle, &transaction); 
  
  return ret;
  
}

esp_err_t
mpu9250_spi_write(spi_device_handle_t handle, uint8_t addr, const uint8_t *data, uint32_t length)
{
  spi_transaction_t transaction;
  transaction.flags = 0;
  transaction.cmd = 0;
  transaction.addr = addr & SPIBUS_WRITE;
  transaction.length = length * 8;
  transaction.rxlength = 0;
  transaction.user = NULL;
  transaction.tx_buffer = data;
  transaction.rx_buffer = NULL;
  esp_err_t ret = spi_device_transmit(handle, &transaction); 
  
  return ret;
  
}
