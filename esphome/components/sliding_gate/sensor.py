import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    UNIT_PERCENT,
    ICON_PERCENT
)

sliding_gate_ns = cg.esphome_ns.namespace('sliding_gate')
SlidingGateComponent = sliding_gate_ns.class_('SlidingGateComponent', sensor.Sensor, cg.Component)

CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(
        SlidingGateComponent,
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_PERCENT,
        accuracy_decimals=0,
    )
    .extend({
        cv.Required("pin_dir_0"): cv.All(pins.internal_gpio_input_pin_schema),
        cv.Required("pin_dir_1"): cv.All(pins.internal_gpio_input_pin_schema),
        cv.Required("pin_pos_0"): cv.All(pins.internal_gpio_input_pin_schema),
        cv.Required("pin_pos_1"): cv.All(pins.internal_gpio_input_pin_schema),
        cv.Required("pin_pos_2"): cv.All(pins.internal_gpio_input_pin_schema),
    })
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    
    for pin_name in ['pin_dir_0','pin_dir_1']:
        pin = await cg.gpio_pin_expression(config[pin_name])
        cg.add(var.set_pin(pin_name,pin))
    for pin_name in ['pin_pos_0','pin_pos_1','pin_pos_2']:
        pin = await cg.gpio_pin_expression(config[pin_name])
        cg.add(var.set_pin(pin_name,pin))