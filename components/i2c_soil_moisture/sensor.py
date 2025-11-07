import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_MOISTURE,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
)

from . import I2CSoilMoisture, i2c_soil_moisture_ns

CONF_LIGHT = "light"
CONF_CAPACITANCE = "capacitance"
CONF_DRY_VALUE = "dry_value"
CONF_WET_VALUE = "wet_value"

UNIT_LUX = "lx"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(I2CSoilMoisture),
        cv.Optional(CONF_MOISTURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CAPACITANCE): sensor.sensor_schema(
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_LIGHT): sensor.sensor_schema(
            unit_of_measurement=UNIT_LUX,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DRY_VALUE, default=290): cv.int_range(min=0, max=65535),
        cv.Optional(CONF_WET_VALUE, default=600): cv.int_range(min=0, max=65535),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    if moisture_config := config.get(CONF_MOISTURE):
        sens = await sensor.new_sensor(moisture_config)
        cg.add(parent.set_moisture_sensor(sens))

    if capacitance_config := config.get(CONF_CAPACITANCE):
        sens = await sensor.new_sensor(capacitance_config)
        cg.add(parent.set_capacitance_sensor(sens))

    if temperature_config := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(parent.set_temperature_sensor(sens))

    if light_config := config.get(CONF_LIGHT):
        sens = await sensor.new_sensor(light_config)
        cg.add(parent.set_light_sensor(sens))

    cg.add(parent.set_dry_value(config[CONF_DRY_VALUE]))
    cg.add(parent.set_wet_value(config[CONF_WET_VALUE]))
