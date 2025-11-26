import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import (
    uart,
    text_sensor,
    button,
)  # binary_sensor, output, sensor, switch,
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["text_sensor", 'button']  # 'binary_sensor', 'output', 'sensor',  'switch',
MULTI_CONF = True

vent_axia_sentinel_kinetic_ns = cg.esphome_ns.namespace("vent_axia_sentinel_kinetic")
VentAxiaSentinelKineticComponent = vent_axia_sentinel_kinetic_ns.class_(
    "VentAxiaSentinelKineticComponent", cg.Component, uart.UARTDevice
)

CONF_VentAxiaSentinelKinetic_ID = "vent_axia_sentinel_kinetic_id"

# CONF_DIAGNOSTIC_BUTTON = "diagnostic_button"

CONF_LINE1 = "line1"
CONF_LINE2 = "line2"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VentAxiaSentinelKineticComponent),
            cv.Optional(CONF_LINE1): text_sensor.text_sensor_schema(
                text_sensor.TextSensor
            ),
            cv.Optional(CONF_LINE2): text_sensor.text_sensor_schema(
                text_sensor.TextSensor
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "vent_axia_sentinel_kinetic",
    require_tx=True,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # if CONF_DIAGNOSTIC_BUTTON in config:
    #     btn = await button.new_button(config[CONF_DIAGNOSTIC_BUTTON])
    #     cg.add(btn.set_parent(var))

    if CONF_LINE1 in config:
        sens = await text_sensor.new_text_sensor(config[CONF_LINE1])
        cg.add(var.set_line1(sens))

    if CONF_LINE2 in config:
        sens = await text_sensor.new_text_sensor(config[CONF_LINE2])
        cg.add(var.set_line2(sens))
