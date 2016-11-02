# NuMaker-mbed-NuBrick
This library contains mbed implementation of [NuMaker Brick](http://www.nuvoton.com/hq/support/tool-and-software/development-tool-hardware/numaker-brick/?__locale=en) I2C protocol to communicate with NuMaker Brick slave modules.
To see how this library is used in practice, we created a [NuMaker Brick example](https://developer.mbed.org/teams/Nuvoton/code/NuMaker-mbed-NuBrick-example/) to show how mbed enabled boards could communicate with NuMaker Brick slave modules.

## Support NuMaker Brick slave modules
- Buzzer
- LED
- AHRS
- Sonar
- Temperature & Humidity
- Gas
- IR
- Keys

## HID-like protocol on I2C bus
To communicate with the outside, NuMaker Brick platform defines simplified HID-like protocol on I2C bus. NuMaker Brick slave modules run as I2C slaves.
This library encapsulates master side logic. Through this library, users can communicate with NuMaker Brick slave modules without needing to know the protocol in detail.

## I2C
To communicate with NuMaker Brick slave devices, users need to instantiate respective `NuBrickMaster` objects with `I2C` object passed in to its constructor.
For example, instantiate `NuBrickMasterBuzzer` object to communicate with the NuMaker Brick slave device _Buzzer_.
```
I2C i2c(D14, D15);
NuBrickMasterBuzzer master_buzzer(i2c, true);       // Debug enabled
```

## Reports and Fields
NuMaker Brick slave modules export three types of reports to the outside. Each report consists of one or more fields.

### Types of reports
1. Feature report. Bidirectional. Used to configure NuMaker Brick slave modules.
1. Input report. Unidirectional. Used to receive e.g. sensor data from NuMaker Brick slave modules.
1. Output report. Unidirectional. Used to send e.g. trigger command to NuMaker Brick slave modules.

### Fields
A `NuBrickMaster` object exports associative array interface to access a field. Through the interface, users can:
- Get minimum value of the field
- Get maximum value of the field
- Get/Set value of the field

To access a specific field in a NuMaker Brick slave module, users need to pass field name in _report.field_ format into the C++ [] operator.
Here, _report_ could only be _feature_, _input_, or _output_. For supported _report.field_ names in a NuMaker Brick slave module,
please refer to respective `NuBrickMasterXxx.h` file in this library.

### Example: configure the NuMaker Brick slave module Buzzer

1. Pull in feature report from the module.

    ```
    master_buzzer.pull_feature_report();
    ```
1. Update fields of the feature report locally.

    ```
    master_buzzer["feature.sleep_period"].set_value(100);
    master_buzzer["feature.volume"].set_value(60);          // Volume in %
    master_buzzer["feature.tone"].set_value(196);           // Tone in Hz
    master_buzzer["feature.song"].set_value(0);             // 0 (mono), 1 (Bee)
    master_buzzer["feature.period"].set_value(200);         // Period in ms
    master_buzzer["feature.duty"].set_value(30);            // Duty in %
    master_buzzer["feature.latency"].set_value(3);          // Alarm for time secs
    ```
1. Push out the updated feature report to the module.

    ```
    master_buzzer.push_feature_report();,
    ```
    
### Example: sound the NuMaker Brick slave module Buzzer

1. Update fields of the output report locally.

    ```
    master_buzzer["output.start_flag"].set_value(1);
    master_buzzer["output.stop_flag"].set_value(0);
    ```
1. Push out the updated output report to the module.

    ```
    master_buzzer.push_output_report();
    ```
