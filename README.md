# ESPHome with ESP32 and ES32A08 Expansion Board

This repository provides examples on integrating the ESP32 microcontroller with the ES32A08 expansion board by eletechsup using ESPHome. This combination allows for versatile applications involving sensor input and output management suitable for DIY electronics projects.

![ES32A08 Expansion Board](images/es32a08-top.jpg)

## Board Features
- **RS485 Interface**
- **8x Opto-isolated Inputs**: Low-level trigger, NPN type.
- **8x 10A Relay Outputs**
- **4x 0-5V/10V Voltage Input**
- **4x 0/4-20MA Current Input**
- **4x Buttons**
- **1x 4-bit Digital Tube Display**

## Examples
Below are examples demonstrating how to utilize each feature of the ES32A08 board.

### Base configurations
Some of the features on the board, are possible due to components like shift registers, so first, we need to define them

For outputs, there are 3 of [SN74HC595](https://esphome.io/components/sn74hc595) I/O expanders (shift registers).
```yaml
sn74hc595:
  - id: 'output_hub'
    data_pin: GPIO13
    clock_pin: GPIO27
    latch_pin: GPIO14
    oe_pin: GPIO4
    sr_count: 3
```

For inputs, there is 1 [SN74HC165](https://esphome.io/components/sn74hc165.html) I/O expander (shift register).
```yaml
sn74hc165:
  - id: 'input_hub'
    data_pin: GPIO5
    clock_pin: GPIO17
    load_pin: GPIO16
```


### Buttons
`KEY1` - `GPIO18`  
`KEY2` - `GPIO19`  
`KEY3` - `GPIO21`  
`KEY4` - `GPIO23`

```yaml
binary_sensor:
  - platform: gpio
    id: key_1
    name: "Key 1"
    internal: true
    pin: 
      number: GPIO18
      inverted: true
      mode:
        input: true
        pullup: true
    on_press:
      then:
        ...
```

### Opto-isolated Inputs
The opto-isolated inputs are available using the SN74HC165 shift register and use **pins 0 through 7**.

```yaml
binary_sensor:
  - platform: gpio
    id: input_1
    name: "Input 1"
    pin: 
      sn74hc165: 'input_hub'
      number: 0
      inverted: true
    on_press:
      then:
        ...
```

*Note: make sure that you define the [base configurations](#base-configurations) needed for this to work*

### Relay Outputs
The relays are avaiable using the SN74HC595 shift register and use **pins 16 through 23**.

```yaml
switch:
  - platform: gpio
    id: relay_1
    name: "Relay 1"
    pin:
      sn74hc595: 'output_hub'
      number: 16
```

*Note: make sure that you define the [base configurations](#base-configurations) needed for this to work*

### Status LED
I suggest using [Status LED Light](https://esphome.io/components/light/status_led.html) component, as it allows to share a single LED for indicating the status of the device (when on error/warning state) or as binary light.

```yaml
light: 
  - platform: status_led
    id: board_led
    name: "Board LED"
    pin: GPIO15
    internal: true
```

### Voltage Input
`V1` - `GPIO32`  
`V2` - `GPIO33`  
`V3` - `GPIO25`  
`V4` - `GPIO26`

> [!NOTE]  
> Inputs `V3` (`GPIO25`) and `V4` (`GPIO26`) can **not** be used, when `wifi:` is used.

```yaml
sensor:
  - platform: adc
    id: voltage_1
    name: "Voltage 1"
    update_interval: 30s
    attenuation: auto
    pin:
      number: GPIO32
      mode:
        input: true
    filters:
      # Converting what ESP sees to real value based on resistors that are used on the board
      - lambda: return x / 10.0 * (10.0 + 43.0);
      # Rounding to 2 decimals but feel free to change this if you need more/less pressicion
      - round: 2
      - delta: 0.01
```

## Where to Buy
You can purchase the ES32A08 expansion board from the following links (no affiliation):

[AliExpress](https://www.aliexpress.com/item/1005006202920238.html)  
[Amazon.com](https://www.amazon.com/Multifunction-Expansion-Digital-Arduino-Ethernet/dp/B0CM6HJCVQ?th=1)  
[eBay](https://www.ebay.com/itm/335132721050)  

### TODO
- [ ] RS485 Interface example
- [ ] Current Input example
- [ ] Digital Tube Display example
- [ ] Full yaml example

## Contributing
Feel free to fork this repository and contribute by submitting pull requests.

## License
Distributed under the MIT License. See [LICENSE](LICENSE) for more information.
