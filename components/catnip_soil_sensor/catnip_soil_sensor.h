#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace catnip_soil_sensor {

// Register addresses
static const uint8_t I2C_SOIL_MOISTURE_GET_CAPACITANCE = 0x00;
static const uint8_t I2C_SOIL_MOISTURE_SET_ADDRESS = 0x01;
static const uint8_t I2C_SOIL_MOISTURE_GET_ADDRESS = 0x02;
static const uint8_t I2C_SOIL_MOISTURE_MEASURE_LIGHT = 0x03;
static const uint8_t I2C_SOIL_MOISTURE_GET_LIGHT = 0x04;
static const uint8_t I2C_SOIL_MOISTURE_GET_TEMPERATURE = 0x05;
static const uint8_t I2C_SOIL_MOISTURE_RESET = 0x06;
static const uint8_t I2C_SOIL_MOISTURE_GET_VERSION = 0x07;
static const uint8_t I2C_SOIL_MOISTURE_SLEEP = 0x08;
static const uint8_t I2C_SOIL_MOISTURE_GET_BUSY = 0x09;

class I2CSoilMoisture : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_capacitance_sensor(sensor::Sensor *capacitance_sensor) { capacitance_sensor_ = capacitance_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_light_sensor(sensor::Sensor *light_sensor) { light_sensor_ = light_sensor; }

 protected:
  sensor::Sensor *capacitance_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *light_sensor_{nullptr};

  bool read_register_16bit_(uint8_t reg, uint16_t *value);
  bool read_register_8bit_(uint8_t reg, uint8_t *value);
  bool write_register_(uint8_t reg);

  enum State {
    STATE_IDLE,
    STATE_LIGHT_MEASURE_REQUESTED,
  } state_{STATE_IDLE};

  uint32_t light_measurement_start_time_{0};
};

}  // namespace catnip_soil_sensor
}  // namespace esphome
