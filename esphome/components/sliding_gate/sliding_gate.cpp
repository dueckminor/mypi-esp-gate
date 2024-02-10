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
  }
}

void SlidingGateComponent::setup() {
  this->dir_state = -1;
  this->pos = -1;
  this->reported_dir = -1;
  this->reported_pos = -1;

  this->pin_dir_0_->setup();
  this->pin_dir_1_->setup();
  this->pin_pos_0_->setup();
  this->pin_pos_1_->setup();
  this->pin_pos_2_->setup();

  this->pin_dir_0_isr = this->pin_dir_0_->to_isr();
  this->pin_dir_1_isr = this->pin_dir_1_->to_isr();

  // this->pin_pos_0_isr = this->pin_pos_0_->to_isr();
  // this->pin_pos_1_isr = this->pin_pos_1_->to_isr();
  // this->pin_pos_2_isr = this->pin_pos_2_->to_isr();

  this->pin_dir_0_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);
  this->pin_dir_1_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);
  // this->pin_pos_0_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);
  // this->pin_pos_1_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);
  // this->pin_pos_2_->attach_interrupt<SlidingGateComponent>(&SlidingGateComponent::handle_interrupt, this, gpio::INTERRUPT_ANY_EDGE);
}

void SlidingGateComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SlidingGate:");
  LOG_PIN("  Pin-Dir-0: ", this->pin_dir_0_);
  LOG_PIN("  Pin-Dir-1: ", this->pin_dir_1_);
  LOG_PIN("  Pin-Pos-0: ", this->pin_pos_0_);
  LOG_PIN("  Pin-Pos-1: ", this->pin_pos_1_);
  LOG_PIN("  Pin-Pos-2: ", this->pin_pos_2_);
}

// float SlidingGateComponent::get_setup_priority() {
  
// }

void SlidingGateComponent::loop() {
  if (this->reported_dir != this->dir_state) {
    ESP_LOGD(TAG, "dir_state %x", this->dir_state);
    this->reported_dir = this->dir_state;
    // ESP_LOGD(TAG,"pin_pos_0: %x",this->pin_pos_0_->digital_read());
    // ESP_LOGD(TAG,"pin_pos_1: %x",this->pin_pos_1_->digital_read());
    // ESP_LOGD(TAG,"pin_pos_2: %x",this->pin_pos_2_->digital_read());
  }
  if (this->reported_pos != this->pos) {
    ESP_LOGD(TAG, "pos %x", this->pos);
    this->reported_pos = this->pos;
  }
}

void IRAM_ATTR SlidingGateComponent::handle_interrupt(SlidingGateComponent *_this) {
  int new_dir_state = 0;

  _this->count++;

  if (_this->pin_dir_0_->digital_read()) {
    new_dir_state |= 1;
  }
  if (_this->pin_dir_1_->digital_read()) {
    new_dir_state |= 2;
  }
  if (new_dir_state == _this->dir_state) {
    return;
  }

  _this->dir_state = new_dir_state;

  if (new_dir_state != 3) {
   return;
  }
  int new_pos = 0;
  if (_this->pin_pos_0_->digital_read()) {
    new_pos |= 1;
  }
  if (_this->pin_pos_1_->digital_read()) {
    new_pos |= 2;
  }
  if (_this->pin_pos_2_->digital_read()) {
    new_pos |= 4;
  }
  _this->pos = new_pos;
}

}
}