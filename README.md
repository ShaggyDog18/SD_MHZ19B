# SD_MHZ19B Library for Winsen MH-Z19B NDIR CO2 Module

by **ShaggyDog18@gmail.com**, JULY 2020

github: https://github.com/ShaggyDog18/SD_MHZ19B

Arduino library for **MH-Z19B NDIR CO2 Module** by [Zhengzhou Winsen Electronics Technology Co., Ltd](www.winsen-sensor.com)

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

MH-Z19B NDIR infrared gas module is a common type, small size sensor, using non-dispersive infrared (NDIR) principle to detect the existence of CO2 in the air, with good selectivity, non-oxygen dependent and long life. Built-in temperature compensation; and it has UART output and PWM output. 

This compact library features access to all module features though an abstracted class and methods to manage the module implemented in user-level functionality.

The library does not work with PWM module signal. Communicates with the module by Hardware or Software UART.

CO2 Measurement Range: 0-2000ppm or 0-5000ppm. Accuracy: ±(50ppm+3%).

## Library Methods

- `SD_MHZ19B( Stream& serial )` - Class Constructor
- `~SD_MHZ19B()` - Class Destructor
- `uint16_t getPPM(void)` - read data from the module; returns value of CO2 concentration in ppm; Data are verified and valid (validate by calculating checkSum of the data received). Retunt `FALSE` i.e. ZERO value if communication or CRC error.
- `int8_t getTemp(void)` - returns module temperature in degrees Celcium. Should be called after getPPM() function; otherwise will return a previous value. Rather inaccurate ±2*C; is used for internal compensation.
- `void setAutoCalibration( bool _autoCalib )` - toggles Auto Calibration (Automatic Baseline Correction - ABC) ON/OFF. Set `true` for Enabled/ON; `false` for Disabled/OFF (default). Refer to Auto Calibration Notes below.
- `bool getABCstatus(void)` - gets Auto Calibration ABC status (undocumented feature). Returns `true` for Enabled/ON, `false` - Disabled/OFF.
- `void calibrateZeroPoint(void)` - calibrate Zero point. During the zero point calibration, the sensor must work in stable gas environment (400ppm) for over 20 minutes.
- `bool calibrateSpanPoint( uint16_t _spanPoint )`- do `ZeroPoint` calibration before `Span` calibration. Make sure the sensor worked under a certain level co2 for over 20 minutes. Suggest using 2000ppm as span, at least 1000ppm. Default value is 2000ppm. Returns true if the requested Span value is OK.
- `bool setDetectionRange( uint16_t _detectionRange )` - sets detection range, default is 0~2000ppm. The range could be 2,000-5,000ppm. Returns `true` if the requested range value is OK.
- `uint8_t getStatus(void)` - Reads module status (undocumented feature), Returns 0 if OK; The value is available after successful getPPM() reading only; otherwise, will return previous value; to be called after getPPM(). 

For more details on the library use refer to the example that utilizes major library methods.

## Auto Calibration Notes

if Auto Calibration (Automatic Baseline Correction - ABC) is ON, the sensor itself performs a zero point judgment and automatic calibration procedure intelligently after a continuous operation period. The automatic calibration cycle is every 24 hours after powered on. The zero point for automatic calibration is **400ppm**. 

This function is usually suitable for indoor air quality monitor such as offices and homes, not suitable for greenhouse, farm and refrigeratory where this function should be off. Please do zero calibration timely, such as manual or command calibration.

## Safe levels of CO2

| CO2 Level       | Description                                                |
| --------------- | ---------------------------------------------------------- |
| 250 - 400ppm    | Normal background concentration in outdoor ambient air |
| 400 - 1,000ppm  | Concentrations typical of occupied indoor spaces with good air exchange |
| 1,000-2,000ppm  | Complaints of drowsiness and poor air. |
| 2,000-5,000 ppm | Headaches, sleepiness and stagnant, stale, stuffy air. Poor concentration, loss of attention, increased heart rate... |

## Compatibility

The library developed for Arduino UNO, NANO, Pro Mini, ESP8266, etc.

## Gratitude

**If you use and like this library, please, consider making a small "cup of coffee" donation using [PayPal](https://paypal.me/shaggyDog18/3USD)**

Enjoy!
