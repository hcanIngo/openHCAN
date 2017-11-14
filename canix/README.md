canix: CAN-Firmware-Basis [z.B. canix_rtc_clock, canix_reg_frame_callback, ...](https://github.com/hcanIngo/openHCAN/wiki/HCAN-Real-Time-System)



Warum hier kein Archiv canix.a und das zu den Firmwares linken?
---------------------------------------------------------------  
Der Code hat einige Preprocessor-Direktiven; diese wuerden nicht so
wie im Hauptprogramm verwendet. Daher compiliert jede Firmware all
die .c Files mit ihren Defines und linkt sie.
