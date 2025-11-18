#include "catnip_soil_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace catnip_soil_sensor {

static const char *const TAG = "catnip_soil_sensor";

void I2CSoilMoisture::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2C Soil Moisture Sensor...");

  // Read sensor version to verify communication
  uint8_t version;
  if (this->read_register_8bit_(I2C_SOIL_MOISTURE_GET_VERSION, &version)) {
    ESP_LOGD(TAG, "Sensor version: %d", version);
  } else {
    ESP_LOGW(TAG, "Failed to read sensor version");
    this->mark_failed();
    return;
  }
}

void I2CSoilMoisture::dump_config() {
  ESP_LOGCONFIG(TAG, "I2C Soil Moisture Sensor:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with I2C Soil Moisture Sensor failed!");
  }

  LOG_SENSOR("  ", "Capacitance", this->capacitance_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Light", this->light_sensor_);
}

void I2CSoilMoisture::update() {
  // Handle light sensor state machine
  if (this->state_ == STATE_LIGHT_MEASURE_REQUESTED) {
    // Check if enough time has passed since triggering light measurement
    if (millis() - this->light_measurement_start_time_ >= 1500) {
      // Read light value
      if (this->light_sensor_ != nullptr) {
        uint16_t light_raw;
        if (this->read_register_16bit_(I2C_SOIL_MOISTURE_GET_LIGHT, &light_raw)) {
          ESP_LOGD(TAG, "Got light raw value: %d", light_raw);
          this->light_sensor_->publish_state(light_raw);
        } else {
          ESP_LOGW(TAG, "Failed to read light sensor data");
        }
      }
      this->state_ = STATE_IDLE;
    } else {
      // Not ready yet, wait until next update cycle
      return;
    }
  }

  // Read capacitance
  if (this->capacitance_sensor_ != nullptr) {
    uint16_t capacitance;
    if (this->read_register_16bit_(I2C_SOIL_MOISTURE_GET_CAPACITANCE, &capacitance)) {
      ESP_LOGD(TAG, "Got capacitance: %d", capacitance);
      this->capacitance_sensor_->publish_state(capacitance);
    } else {
      ESP_LOGW(TAG, "Failed to read capacitance data");
    }
  }

  // Read temperature
  if (this->temperature_sensor_ != nullptr) {
    uint16_t temp_raw;
    if (this->read_register_16bit_(I2C_SOIL_MOISTURE_GET_TEMPERATURE, &temp_raw)) {
      // Temperature is in tenths of degrees Celsius
      float temperature = temp_raw / 10.0f;
      ESP_LOGD(TAG, "Got temperature: %.1f°C", temperature);
      this->temperature_sensor_->publish_state(temperature);
    } else {
      ESP_LOGW(TAG, "Failed to read temperature data");
    }
  }

  // Trigger light measurement if needed
  if (this->light_sensor_ != nullptr && this->state_ == STATE_IDLE) {
    if (this->write_register_(I2C_SOIL_MOISTURE_MEASURE_LIGHT)) {
      ESP_LOGD(TAG, "Light measurement triggered");
      this->state_ = STATE_LIGHT_MEASURE_REQUESTED;
      this->light_measurement_start_time_ = millis();
    } else {
      ESP_LOGW(TAG, "Failed to trigger light measurement");
    }
  }
}

bool I2CSoilMoisture::read_register_16bit_(uint8_t reg, uint16_t *value) {
  // Write register address
  if (this->write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to write register address 0x%02X", reg);
    return false;
  }

  // Wait a bit for the sensor to process
  delay(20);

  // Read 2 bytes (MSB first)
  uint8_t data[2];
  if (this->read(data, 2) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to read 2 bytes from register 0x%02X", reg);
    return false;
  }

  // Combine bytes (MSB first)
  *value = (data[0] << 8) | data[1];
  return true;
}

bool I2CSoilMoisture::read_register_8bit_(uint8_t reg, uint8_t *value) {
  // Write register address
  if (this->write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to write register address 0x%02X", reg);
    return false;
  }

  // Wait a bit for the sensor to process
  delay(20);

  // Read 1 byte
  if (this->read(value, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to read byte from register 0x%02X", reg);
    return false;
  }

  return true;
}

bool I2CSoilMoisture::write_register_(uint8_t reg) {
  if (this->write(&reg, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to write to register 0x%02X", reg);
    return false;
  }
  return true;
}

}  // namespace catnip_soil_sensor
}  // namespace esphome
