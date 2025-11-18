import esphome.codegen as cg
from esphome.components import i2c

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor"]

catnip_soil_sensor_ns = cg.esphome_ns.namespace("catnip_soil_sensor")
CatnipSoilSensor = catnip_soil_sensor_ns.class_(
    "I2CSoilMoisture", cg.PollingComponent, i2c.I2CDevice
)
