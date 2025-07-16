[![latest version](https://img.shields.io/github/release/alextrical/ESPHome-Vent-Axia-Sentinel-Kinetic?display_name=tag&include_prereleases&label=latest%20version)](https://github.com/alextrical/ESPHome-Vent-Axia-Sentinel-Kinetic/releases)

<a href="https://esphome.io/">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://esphome.io/_static/logo-text-on-dark.svg", alt="ESPHome Logo">
    <img src="https://esphome.io/_static/logo-text-on-light.svg" alt="ESPHome Logo">
  </picture>
</a>

## 💬 Feedback & Support

🐞 Found a bug? Let us know via an [Issue](https://github.com/alextrical/ESPHome-Vent-Axia-Sentinel-Kinetic/issues).
💡 Have a question or suggestion? Join the [Discussion](https://github.com/alextrical/ESPHome-Vent-Axia-Sentinel-Kinetic/discussions)!

# Vent Axia Sentinel Kinetic ESPHome

> [!WARNING]
> Due to a change in ESPHome 2025.2.0, some users are reporting build problems related to the loading of the `uptime_seconds_sensor` class. If you get a compile error for this reason, manually add an uptime sensor to your YAML configuration as below, clean your build files, and recompile. Once the root cause is identified this note will be removed.
>
> ```yaml
> sensor:
>   - platform: uptime
>     name: Uptime
> ```

This project is a firmware for ESP32 (and possibly ESP8266) microcontrollers supporting UART communication via the BMS connector. Its purpose is to enable complete control of a compatible Vent Axia Sentinel Kinetic MVHR through Home Assistant, a web interface, or any MQTT client.

It uses the ESPHome framework and is compatible with the Arduino framework and ESP-IDF.

The benefits include fully local control over your MVHR, without reliance on a vendor network. Additional visibility, finer control, and even improved energy efficiency and comfort are possible when utilizing Home Assistant automations.

> [!CAUTION]
> Use at your own risk.
> This is an unofficial implementation of the reverse-engineered Vent Axia protocol based on the information from the [Vent Axia Remote](https://github.com/aelias-eu/vent-axia-remotehttps:/) repo. The authors and contributors have extensively tested this firmware across several similar implementations and forks. However, it's important to note that not all units support every feature. While free to use, it is at your own risk.

### Features

- Wireless Vent Avia Sentinel Kinetic MVHR equipment control via ESP32, using the [ESPHome](https://esphome.io) framework.
- Byte-by-byte reading within the loop() function ensures no data loss or lag, as the component continuously reads without blocking ESPHome.
- UART writes are followed by non-blocking reads. The responses are accumulated byte-by-byte in the loop() method and processed when complete, allowing command stacking without delays for a more responsive UI.
- Diagnostic sensors exposed from the MVHR ro reading temperature, motor speed, humidity, remaining filter life.
- Ongoing refactoring to further improve the code quality.

## Requirements

- [ESPHome](https://esphome.io/) - Minimum version 1.18.0, installed independently or as an add-on in HomeAssistant

## Supported Microcontrollers

> [!IMPORTANT]
> ESP8266 boards may work, but support is no longer actively maintained by this component, if you are purchasing new hardware for use with this project, it is recommended to focus on the more modern and powerful ESP32 based devices.

- [ESP32 Sentinel Kinetic Wireless Dongle](https://github.com/alextrical/ESP32-Sentinel-Kinetic-Wireless-Dongle)

## Supported Vent Axia Sentinel Kinetic MVHR Units

Generally, any MVHR that can use the [Sentinel Kinetic Wired Remote Controller](https://www.vent-axia.com/range/sentinel-kinetic-wired-remote-controllerhttps:/) will be compatible.

Units tested by project contributors include:

- `Vent Axia Sentinel Kinetic BH Unit (RH) - 443319`
- `Vent Axia Sentinel Kinetic BH Unit (LH) - 443319L`
- `Vent Axia Sentinel Kinetic Plus B Heat Recovery (RH) Unit - 443028`
- `Vent Axia Sentinel Kinetic Plus B Heat Recovery (LH) Unit - 443028L`

## Usage

### Step 1: Building the Control Circuit

Follow the [ESP32-Sentinel-Kinetic-Wireless-Dongle](https://github.com/alextrical/ESP32-Sentinel-Kinetic-Wireless-Dongle) for building a control circuit using the ESP32.

### Step 2: Using ESPHome

Add a new device in your ESPHome dashboard. Create a yaml configuration file for the new device using the templates below, and flash to your device. Refer to the ESPHome documentation for guides on how to install ESPHome, add new devices, and flash the initial firmware.

- [Getting Started with ESPHome and HomeAssistant](https://esphome.io/guides/getting_started_hassio)
- [Installing ESPHome Locally](https://esphome.io/guides/installing_esphome)

> [!NOTE]
> This code uses the ESPHome [external components](https://esphome.io/components/external_components.html) integration feature. This means the project is not part of the ESPHome framework, it is an external component not managed by the core ESPHome project.

### Step 3: Configure the board and UART settings

Your ESPHome device configuration file starts with common defaults for ESPHome. To these defaults, add these minimum sections:

```yaml
esp32:
  board: esp32dev
  framework:
    type: esp-idf

uart:
  tx_pin: 17
  rx_pin: 16
  baud_rate: 9600
  id: vask_uart
```

### Step 4: Configure the external component

Add these sections to load the external component, setup logging, and enable the climate entity.

```yaml
# External component reference
external_components:
  - source: github://alextrical/ESPHome-Vent-Axia-Sentinel-Kinetic/@latest

# External component configuration
vent_axia_sentinel_kinetic:
  - id: vask
    uart_id: vask_uart

# Default logging level
logger:
  level: INFO
```

<!-- #### Adjusting the `update_interval`

An ESPHome firmware implements the esphome::Component interface to be integrated into the Inversion Of Control mechanism of the ESPHome framework.
The main method of this process is the `loop()` method. MitsubishiCN105ESPHome performs a series of exchanges with the heat pump through a cycle. This cycle is timed, and its duration is displayed in the logs, provided the `CYCLE` logger is set to at least `INFO`.

If this is the case, you will see logs in the form:

```
[09:48:36][I][CYCLE:052]: 6: Cycle ended in 1.2 seconds (with timeout?: NO)
```

This will give you a good idea of your microcontroller's performance in completing an entire cycle. It is unnecessary to set the `update_interval` below this value.
In this example, setting an `update_interval` to 1500ms could be a fine tuned value.
-->
### Step 5: Optional components and variables

These optional additional configurations add customization and additional capabilities. The examples below assume you have added a substitutions component to your configuration file to allow for easy renaming, and that you have added a `secrets.yaml` file to your ESPHome configuration to hide private variables like your random API keys, OTA passwords, and Wifi passwords.

```yaml
substitutions:
  name: mvhr-1 # Do not use underscores, which are not fully compatible with mDNS
  friendly_name: My MVHR 1
``` 


### Step 6: Build the project and install

Build the project in ESPHome and install to your device. Install the device in your MVHR unit connected to the BMS port, and confirm that it powers up and connects to the Wifi. Visit the local IP address of the device, and confirm that you can change modes and temperature setpoints. HomeAssistant should now include your MVHR.

## Example Configuration - Complete

This example includes all the bells and whistles, optional components, remote temperature sensing, reboot button, and additional sensors in HomeAssistant including uptime, the current wifi SSID, and signal strength. Utilizes a `secrets.yaml` file to store your credentials.

<details>

<summary>Complete Configuration</summary>

```yaml
substitutions:
  name: mvhr-1
  friendly_name: My MVHR 1

esp32:
  board: esp32dev

esphome:
  name: ${name}
  friendly_name: ${friendly_name}

uart:
  tx_pin: 17
  rx_pin: 16
  baud_rate: 9600
  id: vask_uart

api:
  encryption:
    key: !secret api_encryption_key

ota:
  - platform: esphome # Required for ESPhome 2024.6.0 and greater
    password: !secret ota_pass

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  
  ap:
    ssid: "${friendly_name} ESP"
    password: !secret fallback_password

captive_portal:

logger:
  level: INFO

web_server:
  version: 3
  sorting_groups:
    - id: sorting_group_control
      name: "Control Panel"
      sorting_weight: 10
    # - id: sorting_group_number_settings
    #   name: "Number settings"
    #   sorting_weight: 20

# Sync time with Home Assistant.
time:
  - platform: homeassistant
    id: homeassistant_time

# Text sensors with general information.
text_sensor:
  # Expose ESPHome version as sensor.
  - platform: version
    name: ESPHome Version
  # Expose WiFi information as sensors.
  - platform: wifi_info
    ip_address:
      name: IP
    ssid:
      name: SSID
    bssid:
      name: BSSID

# Sensors with general information.
sensor:
  # Uptime sensor.
  - platform: uptime
    name: Uptime

  # WiFi Signal sensor.
  - platform: wifi_signal
    name: WiFi Signal
    update_interval: 120s

external_components:
  # - source: ../components
  - source: github://alextrical/ESPHome-Vent-Axia-Sentinel-Kinetic/@latest
    # components: [ vent_axia_sentinel_kinetic ]
    # refresh: always

status_led:
  pin: GPIO02

vent_axia_sentinel_kinetic:
  - id: vask
    uart_id: vask_uart
    line1:
      id: line_1
      # name: "Line 1"
      web_server:
        sorting_group_id: sorting_group_control
        sorting_weight: 10
      icon: "mdi:text-short"
    line2:
      id: line_2
      # name: "Line 2"
      web_server:
        sorting_group_id: sorting_group_control
        sorting_weight: 20
      icon: "mdi:text-short"
      on_value:
        then:
          - lambda: |-
              if (id(line_1).state.compare(0, 10, "Diagnostic") == 0) {
                  switch (std::stoi(id(line_1).state.substr(12, 2))) {
                      case 0:
                          id(internal_fan_speed).publish_state(std::stoi(x.substr(0, 3)));
                          id(internal_pwm).publish_state(std::stoi(x.substr(4, 3)));
                          id(internal_rpm).publish_state(std::stoi(x.substr(10, 4)));
                          break;
                      case 1:
                          id(external_fan_speed).publish_state(std::stoi(x.substr(0, 3)));
                          id(external_pwm).publish_state(std::stoi(x.substr(4, 3)));
                          id(external_rpm).publish_state(std::stoi(x.substr(10, 4)));
                          break;
                      case 2:
                          id(internal_temp_sensor).publish_state(std::stoi(x.substr(1, 2)));
                          break;
                      case 3:
                          id(external_temp_sensor).publish_state(std::stoi(x.substr(1, 2)));
                          //ESP_LOGD("main", "The current version is %d", std::stoi(x.substr(1, 2)) );
                          break;
                      case 4:
                          id(internal_humidity_sensor).publish_state(std::stoi(x.substr(0, 2)));
                          id(internal_temp_sensor2).publish_state(std::stoi(x.substr(5, 2)));
                          id(internal_humidity_sensor_5min).publish_state(std::stoi(x.substr(10, 2)));
                          break;
                      case 19:
                          id(rail_24v).publish_state(std::stoi(x.substr(0, 1)));
                          break;
                      case 23:
                          id(filter_hours_remain).publish_state(std::stoi(x.substr(0, 5)));
                          //ESP_LOGD("main", "The current version is %d", std::stoi(x.substr(0, 5)) );
                          break;
                      default:
                          // Handle other cases
                          break;
                  }
              }

script:
  - id: enter_and_run_diagnostic
    mode: single
    then:
      - switch.turn_off: down_sw
      - switch.turn_off: set_sw
      - if:
          condition:
            lambda: 'return id(line_1).state.compare(0, 10, "Diagnostic") != 0;'
          then:
            - switch.turn_on: up_sw
            - switch.turn_on: main_sw
            - wait_until:
                condition:
                  lambda: 'return id(line_1).state.compare(0, 10, "Diagnostic") == 0;'
                timeout: 8s
            - switch.turn_off: up_sw
            - switch.turn_off: main_sw
      - while:
          condition:
            lambda: |-
              return true;
          then:
            - switch.turn_on: down_sw
            - wait_until:
                condition:
                  lambda: 'return std::stoi(id(line_1).state.substr(12, 2)) > 23;'
                timeout: 8s
            - switch.turn_off: down_sw
            - switch.turn_on: up_sw
            - wait_until:
                condition:
                  lambda: 'return std::stoi(id(line_1).state.substr(12, 2)) == 0;'
                timeout: 8s
            - switch.turn_off: up_sw
  - id: exit_diagnostic
    mode: single
    then:
      - switch.turn_on: up_sw #Scroll up until we see Diagnostic 0
      - switch.turn_off: down_sw
      - switch.turn_off: set_sw
      - switch.turn_off: main_sw
      - wait_until:
          condition:
            lambda: 'return std::stoi(id(line_1).state.substr(12, 2)) == 0;'
          timeout: 8s
      - switch.turn_off: up_sw
      - delay: 50ms
      - switch.turn_on: up_sw
      - wait_until:
          condition:
            lambda: 'return id(line_1).state.compare(0, 10, "Diagnostic") != 0;'
          timeout: 8s
      - switch.turn_off: up_sw

switch:
  - platform: gpio
    pin: GPIO22
    name: "Set Low"
    icon: "mdi:speedometer-slow"
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 110
  - platform: gpio
    pin: GPIO25
    name: "Set Normal"
    icon: "mdi:speedometer-medium"
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 120
  - platform: gpio
    pin: GPIO33
    name: "Set Boost"
    icon: "mdi:speedometer"
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 130
  - platform: gpio
    pin: 
      number: GPIO21
      inverted: true
    id: fan_power_switch
  - platform: vent_axia_sentinel_kinetic
    up:
      id: up_sw
      # name: "Up"
      entity_category: config
      icon: "mdi:menu-up"
      web_server:
        sorting_group_id: sorting_group_control
        sorting_weight: 70
    down:
      id: down_sw
      # name: "Down"
      entity_category: config
      icon: "mdi:menu-down"
      web_server:
        sorting_group_id: sorting_group_control
        sorting_weight: 80
    set:
      id: set_sw
      # name: "Set"
      entity_category: config
      icon: "mdi:menu-right"
      web_server:
        sorting_group_id: sorting_group_control
        sorting_weight: 90
    main:
      id: main_sw
      # name: "Main"
      entity_category: config
      icon: "mdi:fan"
      web_server:
        sorting_group_id: sorting_group_control
        sorting_weight: 100

button:
  - platform: template
    # name: "Up"
    id: btn_up
    on_press:
      - switch.turn_on: up_sw
      - delay: 50ms
      - switch.turn_off: up_sw
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 30
    icon: "mdi:menu-up-outline"
  - platform: template
    # name: "Down"
    id: btn_down
    on_press:
      - switch.turn_on: down_sw
      - delay: 50ms
      - switch.turn_off: down_sw
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 40
    icon: "mdi:menu-down-outline"
  - platform: template
    # name: "Set"
    id: btn_set
    on_press:
      - switch.turn_on: set_sw
      - delay: 50ms
      - switch.turn_off: set_sw
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 50
    icon: "mdi:menu-right-outline"
  - platform: template
    # name: "Main"
    id: btn_main
    on_press:
      - switch.turn_on: main_sw
      - delay: 50ms
      - switch.turn_off: main_sw
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 60
    icon: "mdi:fan"
  - platform: template
    id: diagnostic_on
    name: "Diagnostic On"
    icon: "mdi:stethoscope"
    on_press:
      - script.execute: enter_and_run_diagnostic
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 104
  - platform: template
    id: diagnostic_off
    name: "Diagnostic Off"
    icon: "mdi:stethoscope"
    on_press:
      - script.stop: enter_and_run_diagnostic
      - script.execute: exit_diagnostic
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 108
      # Create a button to restart the unit from HomeAssistant. Rarely needed, but can be handy.
  - platform: restart
    name: "Restart ${friendly_name}"

fan:
  - platform: speed
    name: "Fan"
    output: pwm_output
    restore_mode: RESTORE_DEFAULT_ON
    on_turn_on: 
      then:
        - switch.turn_on: fan_power_switch
    on_turn_off: 
      then:
        - switch.turn_off: fan_power_switch
    icon: "mdi:fan"
    web_server:
      sorting_group_id: sorting_group_control
      sorting_weight: 5

output:
  - platform: ledc
    pin: GPIO32
    frequency: 500 Hz
    id: pwm_output

sensor:
  - platform: template
    id: external_fan_speed
    name: "External Fan Speed"
    accuracy_decimals: 0
    unit_of_measurement: "%"
    state_class: measurement
    update_interval: never
  - platform: template
    id: internal_fan_speed
    name: "Internal Fan Speed"
    accuracy_decimals: 0
    unit_of_measurement: "%"
    state_class: measurement
    update_interval: never
  - platform: template
    id: internal_temp_sensor
    name: "Internal Temperature"
    accuracy_decimals: 0
    unit_of_measurement: "°C"
    device_class: temperature
    state_class: measurement
    update_interval: never
    icon: "mdi:home-thermometer-outline"
  - platform: template
    id: internal_temp_sensor2
    name: "Internal Temperature 2"
    accuracy_decimals: 0
    unit_of_measurement: "°C"
    device_class: temperature
    state_class: measurement
    update_interval: never
    icon: "mdi:home-thermometer-outline"
  - platform: template
    id: external_temp_sensor
    name: "External Temperature"
    accuracy_decimals: 0
    unit_of_measurement: "°C"
    device_class: temperature
    state_class: measurement
    update_interval: never
    icon: "mdi:sun-thermometer-outline"
  - platform: template
    id: internal_humidity_sensor
    name: "Internal Humidity"
    accuracy_decimals: 0
    unit_of_measurement: "%"
    device_class: humidity
    state_class: measurement
    update_interval: never
    icon: "mdi:water-percent"
  - platform: template
    id: internal_humidity_sensor_5min
    name: "Internal Humidity 5Min"
    accuracy_decimals: 0
    unit_of_measurement: "%"
    device_class: humidity
    state_class: measurement
    update_interval: never
    icon: "mdi:water-percent"
  - platform: template
    id: internal_rpm
    name: "Internal Speed"
    accuracy_decimals: 0
    unit_of_measurement: "RPM"
    state_class: measurement
    update_interval: never
    icon: "mdi:fan-speed-1"
  - platform: template
    id: external_rpm
    name: "External Speed"
    accuracy_decimals: 0
    unit_of_measurement: "RPM"
    state_class: measurement
    update_interval: never
    icon: "mdi:fan-speed-2"
  - platform: template
    id: internal_pwm
    name: "Internal PWM"
    accuracy_decimals: 0
    unit_of_measurement: "%"
    state_class: measurement
    update_interval: never
    icon: "mdi:fan-speed-1"
  - platform: template
    id: external_pwm
    name: "External PWM"
    accuracy_decimals: 0
    unit_of_measurement: "%"
    state_class: measurement
    update_interval: never
    icon: "mdi:fan-speed-2"
  - platform: template
    id: filter_hours_remain
    name: "Remaining Filter Hours"
    accuracy_decimals: 0
    unit_of_measurement: "h"
    device_class: duration
    state_class: measurement
    update_interval: never
    icon: "mdi:clock-outline"

binary_sensor:
  - platform: template
    id: rail_24v
    name: "24V Rail"
    icon: "mdi:flash-triangle-outline"
````

</details>

## Developing, Testing & Debugging

For Mac and Linux users its possible to compile ESPHome project on your workstation, simply by running the comands below in a terminal

Create/activate environment by running from project root:
```
source scripts/setup_build_env.sh
```

To Build ESP32 firmware on your local machine:
```
esphome compile config/VASK-ESP32-Dongle-V1_0.yaml
```

## Reference Documentation

Refer to these for further understanding:

- [ESPHome Custom Sensors](https://esphome.io/components/sensor/custom.html)
- [ESPHome External Components](https://esphome.io/components/external_components.html)

---
