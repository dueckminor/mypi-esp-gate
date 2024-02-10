#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/automation.h"
#include "esphome/core/controller.h"

namespace esphome {
namespace sliding_gate {

/// This class implements support for the Tx20 Wind sensor.
class SlidingGateComponent : public sensor::Sensor, public Component {
 public:

  void set_pin(const std::string &pin_name, InternalGPIOPin *pin);

  void setup() override;
  void dump_config() override;
  //float get_setup_priority() const override;
  void loop() override;

  static void handle_interrupt(SlidingGateComponent *_this);

 protected:
  InternalGPIOPin *pin_dir_0_;
  InternalGPIOPin *pin_dir_1_;
  ISRInternalGPIOPin pin_dir_0_isr;
  ISRInternalGPIOPin pin_dir_1_isr;

  InternalGPIOPin *pin_pos_0_;
  ISRInternalGPIOPin pin_pos_0_isr;
  InternalGPIOPin *pin_pos_1_;
  ISRInternalGPIOPin pin_pos_1_isr;
  InternalGPIOPin *pin_pos_2_;
  ISRInternalGPIOPin pin_pos_2_isr;

  volatile int dir_state;
  volatile int pos;
  volatile int count;

  int reported_dir;
  int reported_pos;
};

}  // namespace tx20
}  // namespace esphome