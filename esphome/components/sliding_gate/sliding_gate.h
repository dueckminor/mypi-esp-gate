#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/automation.h"
#include "esphome/core/controller.h"
#include "esphome/components/cover/cover.h"

namespace esphome {
namespace sliding_gate {

// This class implements support for a sliding gate with a 5 bit hal-sensor
// position and direction detection
class SlidingGateComponent : public cover::Cover, public Component {
 public:

  void set_pin(const std::string &pin_name, InternalGPIOPin *pin);

  void setup() override;
  void dump_config() override;
  //float get_setup_priority() const override;
  void loop() override;
  
  virtual cover::CoverTraits get_traits();
  virtual void control(const cover::CoverCall &call);
  virtual void publish(bool force=false);

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

  InternalGPIOPin *pin_relay_;

protected: // detection
  volatile int detected_dir_bits;
  volatile int detected_pos_bits;
  volatile int detected_motion;
  volatile cover::CoverOperation detected_operation;
  unsigned int detected_motion_millis;

protected:
  float reported_position;
  cover::CoverOperation reported_operation;

protected: // timing
  unsigned long now;

protected: // control
  virtual void set_operation(cover::CoverOperation operation);
  cover::CoverOperation operation_next;
  int control_tries_remaining;
  unsigned int control_millis;
  float control_target_position;
  bool control_force_check;

  virtual void control_check();

protected:   // the relay
  bool relay_state; // the state of the relay
  unsigned int relay_millis; // the time where the relay has been activated
  virtual void relay_click();
  virtual void relay_handle_loop();

};

}  // namespace tx20
}  // namespace esphome