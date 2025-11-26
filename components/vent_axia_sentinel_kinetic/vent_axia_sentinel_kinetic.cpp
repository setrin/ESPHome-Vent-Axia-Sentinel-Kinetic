#include "esphome/core/log.h"
#include "esp_timer.h"
#include "vent_axia_sentinel_kinetic.h"
#include <cctype>


#define highbyte(val)(uint8_t)((val) >> 8)
#define lowbyte(val)(uint8_t)((val) & 0xff)

namespace esphome {
  namespace vent_axia_sentinel_kinetic {

    static const char * TAG = "vent_axia_sentinel_kinetic.component";
    static TaskHandle_t uart_tx_task_handle = nullptr;

    VentAxiaSentinelKineticComponent* VentAxiaSentinelKineticComponent::instance = nullptr;

    void VentAxiaSentinelKineticComponent::periodic_timer_cb(void* arg) {
      if (VentAxiaSentinelKineticComponent::instance) {
        VentAxiaSentinelKineticComponent::instance->send_command_();
      }
    }

    void VentAxiaSentinelKineticComponent::setup() {
      instance = this;      
      ESP_LOGCONFIG(TAG, "esp_timer init");

      // create esp_timer args
      esp_timer_create_args_t periodic_timer_args = {};
      periodic_timer_args.callback = &VentAxiaSentinelKineticComponent::periodic_timer_cb;
      periodic_timer_args.arg = this;
      periodic_timer_args.name = "vak_periodic";

      esp_err_t err = esp_timer_create(&periodic_timer_args, &this->periodic_timer_);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create failed: %d", err);
      }

      this->send_alive_str_();
      ESP_LOGCONFIG(TAG, "VentAxiaSentinelKinetic setup complete.");
    }

    void VentAxiaSentinelKineticComponent::loop() {
      //Send serial packets
      if (CMD_KEY_DATA != 0) {
        if (CMD_KEY_DATA != LAST_CMD_KEY_DATA_) {
          this->calculate_command_(CMD_KEY_HEADER, CMD_KEY_DATA);
          LAST_CMD_KEY_DATA_ = CMD_KEY_DATA;
        }

        if (!esp_timer_is_active(this->periodic_timer_)) {
          // start periodic timer with 26000 us (26 ms)
          esp_err_t start_err = esp_timer_start_periodic(this->periodic_timer_, 26000);
          if (start_err != ESP_OK) {
            ESP_LOGE(TAG, "esp_timer_start_periodic failed: %d", start_err);
          }
        }

      } else {
        if (CMD_KEY_DATA != LAST_CMD_KEY_DATA_) {
          if (esp_timer_is_active(this->periodic_timer_)) {
            esp_err_t stop_err = esp_timer_stop(this->periodic_timer_);
            if (stop_err != ESP_OK) {
              ESP_LOGE(TAG, "esp_timer_stop failed: %d", stop_err);
            }
          }
        }
      }

      //Recieve Serial packets
      while (this->available() != 0) {
        uint8_t c;
        this->read_byte( & c);

        if (this->current_index_ == 0 && c != 0x02) {
          return; // Wait for header
        }

        this->buffer_[this->current_index_++] = c;

        if (this->current_index_ == sizeof(this->buffer_)) {
          if (this->validate_crc_()) {
            // this->packet_ready_ = true;
            this->process_packet_();
          }
          this->current_index_ = 0;
        }
      }
    }

    void VentAxiaSentinelKineticComponent::dump_config() {
      this->check_uart_settings(9600, 1, esphome::uart::UART_CONFIG_PARITY_NONE, 8);
      ESP_LOGCONFIG(TAG, "Vent Axia Sentinel Kinetic Component");
      if (this->is_failed()) {
        ESP_LOGE(TAG, "Connection with VentAxiaSentinelKinetic failed!");
      }
      LOG_TEXT_SENSOR("", "Line1 Sensor", this->line1_);
      LOG_TEXT_SENSOR("", "Line2 Sensor", this->line2_);
      #ifdef USE_SWITCH
      LOG_SWITCH("  ", "UpSwitch", this->up_switch_);
      LOG_SWITCH("  ", "DownSwitch", this->down_switch_);
      LOG_SWITCH("  ", "SetSwitch", this->set_switch_);
      LOG_SWITCH("  ", "MainSwitch", this->main_switch_);
      #endif
    }

    void VentAxiaSentinelKineticComponent::send_alive_str_() {
      this->calculate_command_(CMD_ALIVE_HEADER, CMD_ALIVE_DATA);
    }

    void VentAxiaSentinelKineticComponent::calculate_command_(const uint8_t * command_value, uint8_t command) {
      this->cmdbuffer_[0] = CMD_FRAME_HEADER;
      // command value bytes
      if (command_value != nullptr) {
        for (int i = 0; i < 4; i++) {
          this->cmdbuffer_[i + 1] = command_value[i];
        }
      }
      this->cmdbuffer_[5] = command;

      uint16_t crc = 0xFFFF;
      for (uint32_t i = 0; i < 6; i++) {
        crc -= this->cmdbuffer_[i];
      }
      this->cmdbuffer_[6] = highbyte(crc);
      this->cmdbuffer_[7] = lowbyte(crc);
      send_command_();
    }

    void VentAxiaSentinelKineticComponent::send_command_() {
      this->write_array(this->cmdbuffer_, 8);
    }

    bool VentAxiaSentinelKineticComponent::validate_crc_() {
      uint16_t crc = 0xFFFF;
      for (int i = 0; i < 39; i++) {
        crc -= this->buffer_[i];
      }

      uint16_t received_crc = (this->buffer_[39] << 8) | this->buffer_[40];
      return (crc == received_crc);
    }

    void VentAxiaSentinelKineticComponent::set_down(bool enable) {
      CMD_KEY_DATA = (CMD_KEY_DATA & ~1) | enable;
    }
    void VentAxiaSentinelKineticComponent::set_up(bool enable) {
      CMD_KEY_DATA = (CMD_KEY_DATA & ~(1 << 1)) | (enable << 1);
    }
    void VentAxiaSentinelKineticComponent::set_set(bool enable) {
      CMD_KEY_DATA = (CMD_KEY_DATA & ~(1 << 2)) | (enable << 2);
    }
    void VentAxiaSentinelKineticComponent::set_main(bool enable) {
      CMD_KEY_DATA = (CMD_KEY_DATA & ~(1 << 3)) | (enable << 3);
    }

    void VentAxiaSentinelKineticComponent::process_packet_() {
      if (std::memcmp(this->buffer_, this->last_buffer_, sizeof(this->buffer_)) != 0){ //Only process the string if its changed
        std::string buff(reinterpret_cast < const char * > (this->buffer_ + 5), 34);
        
        // Replace non-printable characters with '*'
        for (char& c : buff) {
            if (!isprint(static_cast<unsigned char>(c))) {
                c = '*';
            }
        }

        // std::replace_if(
        //     buff.begin(),
        //     buff.end(),
        //     [](unsigned char c) { return !std::isprint(c); },
        //     '*'
        // );

        std::string new_line1_state = buff.substr(1, 16);
        if (new_line1_state != this->prev_line1_state_) {
            this->line1_->publish_state(new_line1_state.c_str());
            this->prev_line1_state_ = new_line1_state;
        }

        std::string new_line2_state = buff.substr(18, 16);
        if (new_line2_state != this->prev_line2_state_) {
            this->line2_->publish_state(new_line2_state.c_str());
            this->prev_line2_state_ = new_line2_state;
        }

        std::memcpy(this->last_buffer_, this->buffer_, sizeof(this->buffer_));
      }
    }

  } // namespace vent_axia_sentinel_kinetic
} // namespace esphome