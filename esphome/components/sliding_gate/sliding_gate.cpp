#include "esphome.h"
#include "esphome/core/log.h"

#include "sliding_gate.h"

namespace esphome {
namespace sliding_gate {

static const char *const TAG = "sliding_gate";

void SlidingGateComponent::set_pin(const std::string &pin_name, InternalGPIOPin *pin) {
  if (pin_name == "pin_dir_0") {
    this->pin_dir_0_ = pin;
  } else if (pin_name == "pin_dir_1") {
    this->pin_dir_1_ = pin;
  } else if (pin_name == "pin_pos_0") {
    this->pin_pos_0_ = pin;
  } else if (pin_name == "pin_pos_1") {
    this->pin_pos_1_ = pin;
  } else if (pin_name == "pin_pos_2") {
    this->pin_pos_2_ = pin;
  } else if (pin_name == "pin_relay") {
    this->pin_relay_ = pin;
  }
}

void SlidingGateComponent::setup() {
  this->position = 0.0;
  this->control_target_position = 0.0;

  this->detected_dir_bits = -1;
  this->detected_pos_bits = -1;
  this->detected_motion = false;
  this->detected_operation = cover::COVER_OPERATION_IDLE;
  this->current_operation = cover::COVER_OPERATION_IDLE;
  this->operation_next = cover::COVER_OPERATION_IDLE;

  this->reported_operation = cover::COVER_OPERATION_IDLE;
  this->reported_position = -1.0; // this forces that the state gets published

  this->relay_millis = 0;
  this->relay_state = 0;

  this->pin_dir_0_->setup();
  this->pin_dir_1_->setup();
  this->pin_pos_0_->setup();
  this->pin_pos_1_->setup();
  this->pin_pos_2_->setup();

  this->pin_dir_0_isr = this->pin_dir_0_->to_isr();
  this->pin_dir_1_isr = this->pin_dir_1_->to_isr();
  this->pin_pos_0_isr = this->pin_pos_0_->to_isr();
  this->pin_pos_1_isr = this->pin_pos_1_->to_isr();
  this->pin_pos_2_isr = this->pin_pos_2_->to_isr();

  this->pin_dir_0_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);
  this->pin_dir_1_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);

  handle_interrupt(this);


}

void SlidingGateComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SlidingGate:");
  LOG_PIN("  Pin-Dir-0: ", this->pin_dir_0_);
  LOG_PIN("  Pin-Dir-1: ", this->pin_dir_1_);
  LOG_PIN("  Pin-Pos-0: ", this->pin_pos_0_);
  LOG_PIN("  Pin-Pos-1: ", this->pin_pos_1_);
  LOG_PIN("  Pin-Pos-2: ", this->pin_pos_2_);
  LOG_PIN("  Pin-Relay: ", this->pin_relay_);
}

void SlidingGateComponent::loop() {
  this->now = millis();
  this->relay_handle_loop();

  if (this->detected_operation != cover::COVER_OPERATION_IDLE) {
    this->set_operation(this->detected_operation);
    this->detected_operation = cover::COVER_OPERATION_IDLE;
    this->control_force_check = true;
  }

  if (this->detected_motion) {
    this->position = (1.0/7) * (this->detected_pos_bits);
    this->detected_motion_millis = this->now;
    this->detected_motion = false;
  } else if (this->current_operation != cover::COVER_OPERATION_IDLE) {
    if ((this->now - this->detected_motion_millis) > 15000) {
      ESP_LOGD(TAG,"No motion detected since 15 seconds. Assuming idle operation...");
      this->set_operation(cover::COVER_OPERATION_IDLE);
    }
  }

  if (this->position == cover::COVER_CLOSED && this->current_operation == cover::COVER_OPERATION_CLOSING) {
    this->set_operation(cover::COVER_OPERATION_IDLE);
  }
  if (this->position == cover::COVER_OPEN && this->current_operation == cover::COVER_OPERATION_OPENING) {
    this->set_operation(cover::COVER_OPERATION_IDLE);
  }

  this->publish();
  this->control_check();
}


void SlidingGateComponent::publish(bool force)
{
  if (force ||
      (this->reported_position != this->position) || 
      (this->reported_operation != this->current_operation)) {
    this->reported_position = this->position;
    this->reported_operation = this->current_operation;
    this->publish_state();
    //ESP_LOGD(TAG,"Expected Operation: %s",cover::cover_operation_to_str(this->operation_next));
  }
}


void IRAM_ATTR SlidingGateComponent::handle_interrupt(SlidingGateComponent *_this) {
  int new_dir_state = 0;

  new_dir_state |= (_this->pin_dir_0_->digital_read()) ? 1 : 0;
  new_dir_state |= (_this->pin_dir_1_->digital_read()) ? 2 : 0;

  if (new_dir_state == _this->detected_dir_bits) {
    return;
  }

  _this->detected_motion = true;

  if (_this->detected_dir_bits != -1) {
    // This is now (at least) the second time where the dir bits have changed.
    // This allows us to calculate the direction
    static const int state_to_index[] = {0, 1, 3, 2};
    int old_index = state_to_index[_this->detected_dir_bits&3];
    int new_index = state_to_index[new_dir_state];
    switch ((old_index - new_index) & 3) {
    case 1:
      _this->detected_operation = cover::COVER_OPERATION_OPENING;
      break;
    case 3:
      _this->detected_operation = cover::COVER_OPERATION_CLOSING;
      break;
    default:
      _this->detected_operation = cover::COVER_OPERATION_IDLE;
      break;
    }
  }

  _this->detected_dir_bits = new_dir_state;

  if (new_dir_state != 0) {
   return;
  }

  // If new_dir_state is 0, all direction hal-sensors are active.
  // In this case, we can assume that the position bits are correct.

  int new_pos = 0;
  new_pos |= (_this->pin_pos_0_->digital_read()) ? 1 : 0;
  new_pos |= (_this->pin_pos_1_->digital_read()) ? 2 : 0;
  new_pos |= (_this->pin_pos_2_->digital_read()) ? 4 : 0;

  _this->detected_pos_bits = new_pos;
}

cover::CoverTraits SlidingGateComponent::get_traits()
{
  auto traits = cover::CoverTraits();
  traits.set_supports_stop(true);
  traits.set_supports_position(true);
  traits.set_supports_toggle(true);
  return traits;
}

void SlidingGateComponent::control(const cover::CoverCall &call)
{
  // call publish now, to dump the current state in the logs
  this->publish(true);

  if (call.get_stop()) {
    if (this->current_operation != cover::COVER_OPERATION_IDLE) {
      this->relay_click();
    }
    this->control_tries_remaining = 0;
    return;
  } else if (call.get_toggle().has_value()) {
    this->relay_click();
  } else if (call.get_position().has_value()) {
    // go to position action
    this->control_target_position = *call.get_position();
    ESP_LOGD(TAG,"control: control_target_position: %.0f%%",this->control_target_position*100.0f);
    if (this->control_target_position > cover::COVER_OPEN) {
      this->control_target_position = cover::COVER_OPEN;
      ESP_LOGD(TAG,"control: control_target_position: %.0f%%",this->control_target_position*100.0f);
    }
    if (this->control_target_position < cover::COVER_CLOSED) {
      this->control_target_position = cover::COVER_CLOSED;
      ESP_LOGD(TAG,"control: control_target_position: %.0f%%",this->control_target_position*100.0f);
    }
    // We need at most 3 clicks to bring the door in the correct operation.
    // But if our assumption about the current state is wrong, we need
    // some more clicks. 
    this->control_tries_remaining = 5;
    this->control_millis = this->now;
    this->control_force_check = true;
    this->control_check();
  }
}

void SlidingGateComponent::control_check() {
  if (!this->control_tries_remaining) {
    return;
  }

  // cover::COVER_CLOSED = 0.0
  // cover::COVER_OPEN   = 1.0
  
  if ( !this->control_force_check && (this->now - this->control_millis) < 2000 ) {
    return;
  }

  ESP_LOGD(TAG,"control_check:");

  this->control_millis = this->now;
  this->control_force_check = false;

  ESP_LOGD(TAG,"current_position: %.0f%%",this->position * 100.0f);
  ESP_LOGD(TAG,"target_position: %.0f%%",this->control_target_position * 100.0f);
  ESP_LOGD(TAG,"current_operation: %s",cover::cover_operation_to_str(this->current_operation));

  cover::CoverOperation needed_operation = cover::COVER_OPERATION_IDLE;
  
  float diff = this->control_target_position - this->position;

  if (diff < 0) {
    if ((this->control_target_position != cover::COVER_CLOSED) && (diff > -0.1)) {
      needed_operation = cover::COVER_OPERATION_IDLE;
    } else {
      needed_operation = cover::COVER_OPERATION_CLOSING;
    }
  } else if (diff > 0) {
    if ((this->control_target_position != cover::COVER_OPEN) && (diff < 0.1)) {
      needed_operation = cover::COVER_OPERATION_IDLE;
    } else {
      needed_operation = cover::COVER_OPERATION_OPENING;
    }
  }

  ESP_LOGD(TAG,"target_operation: %s",cover::cover_operation_to_str(needed_operation));

  if (needed_operation == this->current_operation) {
    if (needed_operation == cover::COVER_OPERATION_IDLE) {
      this->control_tries_remaining=0;
    }
    return;
  }

  if (this->relay_state) {
    ESP_LOGD(TAG,"relay is busy..."); // -> we have to wait
    return;
  }

  // the current operation is wrong, use one of the remaining clicks
  this->control_tries_remaining--;
  this->relay_click();
}

void SlidingGateComponent::relay_click() {
  if (this->relay_state) {
    return;
  }
  this->relay_state = true;
  this->relay_millis = this->now;
  ESP_LOGD(TAG,"relay: ON");
  this->pin_relay_->digital_write(true);
}

void SlidingGateComponent::relay_handle_loop() {
  if (!this->relay_state || ((this->now - this->relay_millis) < 250)) {
    return;
  }
  this->relay_state = false;
  ESP_LOGD(TAG,"relay: OFF");
  this->pin_relay_->digital_write(false);
  this->set_operation(this->operation_next);
}

void SlidingGateComponent::set_operation(cover::CoverOperation operation)
{
  if (operation == this->current_operation) {
    return;
  }

  if (operation == cover::COVER_OPERATION_IDLE) {
    if (this->current_operation == cover::COVER_OPERATION_OPENING) {
      this->operation_next = cover::COVER_OPERATION_CLOSING;
    } else if (this->current_operation == cover::COVER_OPERATION_CLOSING) {
      this->operation_next = cover::COVER_OPERATION_OPENING;
    } 
  } else {
    this->operation_next = cover::COVER_OPERATION_IDLE;
  }

  this->current_operation = operation;
}

}
}