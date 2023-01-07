#ifndef CONFIG_h
#define CONFIG_h

/* ISO14443 support (for older Aime/Nesica/BANAPASSPORT cards... reader will pretend it was a FeliCa for maximum cardio compatibility) */
#define WITH_ISO14443 1

/* Pinout for the PN5180 free pins */
const int8_t PN5180_PIN_NSS = 10;
const int8_t PN5180_PIN_BUSY = 9;
const int8_t PN5180_PIN_RST = 46;

/* Player ID (1 or 2) */
#define CARDIO_ID 1
/* Enable custom VID/PID (will require to hold reset button to reflash firmware) */
#define CUSTOM_VIDPID 1
#endif
