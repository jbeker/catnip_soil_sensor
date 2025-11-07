# I2C Soil Moisture Sensor for ESPHome

ESPHome external component for the Chirp/Catnip I2C capacitive soil moisture sensor.

Based on the sensor available at: https://github.com/Miceuz/i2c-moisture-sensor

## Features

This component provides the following sensor readings:

- **Moisture** - Calibrated soil moisture percentage (0-100%)
- **Capacitance** - Raw capacitance value for manual calibration
- **Temperature** - Soil temperature in Celsius
- **Light** - Ambient light level (higher values = darker)

## Hardware

### Sensor Specifications

- **I2C Address:** 0x20 (default, configurable)
- **Supply Voltage:** 3.3V - 5V
- **Interface:** I2C
- **Measurements:** Capacitance, Temperature, Light

### Wiring

Connect the sensor to your ESP device:

| Sensor Pin | ESP Pin |
|------------|---------|
| VCC        | 3.3V or 5V |
| GND        | GND |
| SDA        | SDA (GPIO21 on ESP32) |
| SCL        | SCL (GPIO22 on ESP32) |

## Installation

### 1. Copy Component Files

Clone this repository or copy the `components/i2c_soil_moisture` directory to your ESPHome configuration directory.

```
your-esphome-config/
├── components/
│   └── i2c_soil_moisture/
│       ├── __init__.py
│       ├── sensor.py
│       ├── i2c_soil_moisture.h
│       └── i2c_soil_moisture.cpp
└── your-device.yaml
```

### 2. Configure ESPHome

Add the external component and sensor configuration to your ESPHome YAML file:

```yaml
# Enable I2C
i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true

# Load external component
external_components:
  - source:
      type: local
      path: components

# Configure sensor
sensor:
  - platform: i2c_soil_moisture
    address: 0x20
    update_interval: 60s

    moisture:
      name: "Soil Moisture"

    capacitance:
      name: "Soil Capacitance"

    temperature:
      name: "Soil Temperature"

    light:
      name: "Soil Light Level"
```

## Configuration Variables

### Main Configuration

- **address** (*Optional*, int): I2C address of the sensor. Defaults to `0x20`.
- **update_interval** (*Optional*, Time): Update interval for sensor readings. Defaults to `60s`.
- **dry_value** (*Optional*, int): Capacitance value when sensor is completely dry (in air). Used for moisture percentage calculation. Defaults to `290`.
- **wet_value** (*Optional*, int): Capacitance value when sensor is completely wet (in water). Used for moisture percentage calculation. Defaults to `600`.

### Sensor Outputs

All sensor outputs are optional. Configure only the sensors you need.

- **moisture** (*Optional*): Soil moisture as a percentage (0-100%). Calculated from capacitance using dry_value and wet_value.
  - All options from [Sensor](https://esphome.io/components/sensor/index.html#config-sensor)

- **capacitance** (*Optional*): Raw capacitance value. Useful for calibration.
  - All options from [Sensor](https://esphome.io/components/sensor/index.html#config-sensor)

- **temperature** (*Optional*): Soil temperature in Celsius.
  - All options from [Sensor](https://esphome.io/components/sensor/index.html#config-sensor)

- **light** (*Optional*): Ambient light level (higher values indicate darker conditions).
  - All options from [Sensor](https://esphome.io/components/sensor/index.html#config-sensor)

## Calibration

For accurate moisture percentage readings, calibrate your sensor:

1. **Measure Dry Value:**
   - Leave the sensor in open air
   - Read the `capacitance` sensor value
   - Note this as your `dry_value`

2. **Measure Wet Value:**
   - Submerge the sensor in water (only the sensing area, not electronics!)
   - Read the `capacitance` sensor value
   - Note this as your `wet_value`

3. **Update Configuration:**
   ```yaml
   sensor:
     - platform: i2c_soil_moisture
       dry_value: 290  # Your measured dry value
       wet_value: 650  # Your measured wet value
       moisture:
         name: "Soil Moisture"
   ```

## Example Configuration

See [example.yaml](example.yaml) for a complete configuration example.

## Technical Details

### Sensor Registers

The component communicates with the following sensor registers:

| Register | Function | Data Type |
|----------|----------|-----------|
| 0x00 | Get Capacitance | 16-bit (MSB first) |
| 0x05 | Get Temperature | 16-bit (tenths of °C) |
| 0x03 | Trigger Light Measurement | Write only |
| 0x04 | Get Light Value | 16-bit (MSB first) |
| 0x07 | Get Firmware Version | 8-bit |

### Update Cycle

The component performs measurements in the following order:
1. Read capacitance value
2. Calculate and publish moisture percentage (if configured)
3. Read temperature
4. Trigger light measurement
5. Wait ~1.5 seconds
6. Read light value (on next update cycle)

## Troubleshooting

### Sensor Not Found

If you see "Communication with I2C Soil Moisture Sensor failed!" in the logs:

1. Check wiring connections
2. Verify I2C pins are correct for your ESP board
3. Run I2C scan to detect devices: set `scan: true` in i2c configuration
4. Check if sensor is powered (3.3V or 5V)

### Incorrect Readings

- **Moisture always 0% or 100%:** Recalibrate using your specific sensor values
- **Temperature seems wrong:** Sensor reports in tenths of Celsius (252 = 25.2°C)
- **Light values noisy:** This is expected; the light sensor is less precise than other readings

## License

This component is provided as-is for use with ESPHome.

## Credits

- Sensor hardware: [Miceuz/i2c-moisture-sensor](https://github.com/Miceuz/i2c-moisture-sensor)
- ESPHome: [esphome.io](https://esphome.io)
