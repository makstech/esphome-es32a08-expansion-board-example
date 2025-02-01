import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, sn74hc595
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL, UNIT_MS

es32a08_ns = cg.esphome_ns.namespace("es32a08")
ES32A08Display = es32a08_ns.class_("ES32A08Display", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES32A08Display),
    cv.Required("text_id"): cv.use_id(text_sensor.TextSensor),
    cv.Required("sn74hc595"): cv.use_id(sn74hc595.SN74HC595),
    cv.Optional(CONF_UPDATE_INTERVAL, default="25ms"): cv.time_period,
}).extend(cv.polling_component_schema("25ms"))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    text_sensor_id = config["text_id"]
    cg.add(var.set_text_sensor(text_sensor_id))
    sr_id = config["sn74hc595"]
    cg.add(var.set_shift_register(sr_id))
    cg.add(var.set_update_interval(config.get(CONF_UPDATE_INTERVAL)))
