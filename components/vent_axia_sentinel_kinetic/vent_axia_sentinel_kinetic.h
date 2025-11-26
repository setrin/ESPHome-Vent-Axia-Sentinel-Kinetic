#pragma once

#include "esp_timer.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/button/button.h"

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
  namespace vent_axia_sentinel_kinetic {

    // Commands
    static
    const uint8_t CMD_FRAME_HEADER = 0x04;
    static
    const uint8_t CMD_ALIVE_HEADER[4] = {
      0x06,
      0xFF,
      0xFF,
      0xFF
    };
    static
    const uint8_t CMD_KEY_HEADER[4] = {
      0x05,
      0xAF,
      0xEF,
      0xFB
    };
    static
    const uint8_t CMD_ALIVE_DATA = 0x10;
    static uint8_t CMD_KEY_DATA = 0x00;

    class VentAxiaSentinelKineticComponent: public uart::UARTDevice, public Component {
      #ifdef USE_SWITCH
      SUB_SWITCH(up)
      SUB_SWITCH(down)
      SUB_SWITCH(set)
      SUB_SWITCH(main)
      #endif

      public:
        float get_setup_priority() const override {
          return setup_priority::LATE;
        }
        void setup() override;
        void loop() override;
        void dump_config() override;
        void set_up(bool enable);
        void set_down(bool enable);
        void set_set(bool enable);
        void set_main(bool enable);

        void set_line1(text_sensor::TextSensor * text_sensor) {line1_ = text_sensor;}
        void set_line2(text_sensor::TextSensor * text_sensor) {line2_ = text_sensor;}


      protected:
        text_sensor::TextSensor * line1_ {nullptr};
        text_sensor::TextSensor * line2_ {nullptr};
        void calculate_command_(const uint8_t * command_value, uint8_t command_str);
        void send_alive_str_();

        void send_command_();
        int32_t last_periodic_millis_ = millis();
        bool validate_crc_();
        void process_packet_();
        uint8_t current_index_ = 0;
        uint8_t cmdbuffer_[8];
        uint8_t LAST_CMD_KEY_DATA_ = 0x00;

        int32_t toggle_time_;

        uint8_t buffer_[41];
        uint8_t last_buffer_[41];
        std::string prev_line1_state_;
        std::string prev_line2_state_;

        static VentAxiaSentinelKineticComponent* instance;
        static void periodic_timer_cb(void* arg);
        esp_timer_handle_t periodic_timer_ = nullptr;
    };

  } // namespace vent_axia_sentinel_kinetic
} // namespace esphome