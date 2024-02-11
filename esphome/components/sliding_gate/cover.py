import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import cover
from esphome.const import (
    CONF_ID,
)

sliding_gate_ns = cg.esphome_ns.namespace('sliding_gate')
SlidingGateComponent = sliding_gate_ns.class_('SlidingGateComponent', cover.Cover, cg.Component)

CONFIG_SCHEMA = cv.All(
    cover.COVER_SCHEMA
    .extend(cv.COMPONENT_SCHEMA)
    .extend({
        cv.GenerateID(): cv.declare_id(SlidingGateComponent),
        cv.Required("pin_dir_0"): cv.All(pins.internal_gpio_input_pullup_pin_schema),
        cv.Required("pin_pos_0"): cv.All(pins.internal_gpio_input_pin_schema),
        cv.Required("pin_pos_1"): cv.All(pins.internal_gpio_input_pullup_pin_schema),
        cv.Required("pin_pos_2"): cv.All(pins.internal_gpio_input_pullup_pin_schema),
        cv.Required("pin_dir_1"): cv.All(pins.internal_gpio_input_pullup_pin_schema),
        cv.Required("pin_relay"): cv.All(pins.internal_gpio_output_pin_schema),
    })
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)
    
    for pin_name in ['pin_dir_0','pin_dir_1','pin_pos_0','pin_pos_1','pin_pos_2','pin_relay']:
        pin = await cg.gpio_pin_expression(config[pin_name])
        cg.add(var.set_pin(pin_name,pin))
