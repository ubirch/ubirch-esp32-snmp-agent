![ubirch logo](https://ubirch.de/wp-content/uploads/2018/10/cropped-uBirch_Logo.png)

# ESP32 ubirch snmp client

based on [esp32_simple_snmpv2c_agent](https://github.com/leandroadonis86/esp32_simple_snmpv2c_agent)

## Provided MIBs

* temperature      .1.3.6.1.4.1.54021.1.1.0 
    * returns the **Temperature in °C** as an integer value, but actually is floating point value 32 Bit

* humidity         .1.3.6.1.4.1.54021.1.2.0
    * returns the **Humidity in %** as an integer value, but actually is floating point value 32 Bit

* timestamp        .1.3.6.1.4.1.54021.1.3.0
    * returns the **Timestamp in sec.** from 01.01.1970 as an integer value

* uuid             .1.3.6.1.4.1.54021.1.4.0
    * returns the **Hardware ID** as a string of hexadecimal values, which is 16 Bytes long
    
* complete package .1.3.6.1.4.1.54021.1.5.0
    * returns the **complete data package** consisting of
        * uuid (16 Byte)
        * timestamp (4 Byte)
        * temperature (4 Byte)
        * humidity (4 Byte)
    * also triggers the UPP package generation, which will then be transmitted to the ubirch backend

## Verification

To verify the data package, a **base64** encoded **hash512** sum has to 
be created of the data package and then verified against the 
corresponding ubirch endpoint.
```bash
todo example
```


