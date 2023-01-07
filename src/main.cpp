#include <Arduino.h>
#include "Config.h"
#include "PN5180/PN5180.h"
#include "PN5180/PN5180FeliCa.h"
#include "PN5180/PN5180ISO15693.h"
#if WITH_ISO14443 == 1
#include "PN5180/PN5180ISO14443.h"
#endif
#include "CARDIOHID.h"

PN5180FeliCa nfcFeliCa(PN5180_PIN_NSS, PN5180_PIN_BUSY, PN5180_PIN_RST);
PN5180ISO15693 nfc15693(PN5180_PIN_NSS, PN5180_PIN_BUSY, PN5180_PIN_RST);
#if WITH_ISO14443 == 1
PN5180ISO14443 nfc14443(PN5180_PIN_NSS, PN5180_PIN_BUSY, PN5180_PIN_RST);
#endif
USBHID HID;
CARDIOHID_ Cardio(HID);
//SPIClass hspi(HSPI);

void setup() {
	Cardio.begin();	
	USB.manufacturerName("CrazyRedMachine");
	USB.productName("CardIO");
	USB.serialNumber("CARDIOP1");
	USB.begin();
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	//hspi.begin(SCK, MISO, MOSI, SS);
	
	/* NFC */
	nfcFeliCa.begin();
	nfcFeliCa.reset();

	uint8_t productVersion[2];
	nfcFeliCa.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
	if (0xff == productVersion[1]) { // if product version 255, the initialization failed
		Serial.println("NFC initialization failed!");
		exit(-1); // halt
	}
	Serial.printf("Product version %d.%d\n", productVersion[1], productVersion[0]);

	uint8_t firmwareVersion[2];
	nfcFeliCa.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
	Serial.printf("Firmware version %d.%d\n", firmwareVersion[1], firmwareVersion[0]);
	
	uint8_t eepromVersion[2];
	nfcFeliCa.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
	Serial.printf("EEPROM version %d.%d\n", eepromVersion[1], eepromVersion[0]);
	
	nfcFeliCa.setupRF();
}

unsigned long lastReport = 0;
int cardBusy = 0;

// read cards loop
void loop() {
	/* NFC */
	if (millis()-lastReport < cardBusy) return;

	cardBusy = 0;
	uint8_t uid[8] = {0,0,0,0,0,0,0,0};
	uint8_t hid_data[8] = {0,0,0,0,0,0,0,0};

#if WITH_ISO14443 == 1
	// check for ISO14443 card
  nfc14443.reset();
  nfc14443.setupRF();
  uint8_t uidLengthMF = nfc14443.readCardSerial(uid);

  if (uidLengthMF > 0) 
  {
	  Serial.println("ISO14443 card");
      uid[0] &= 0x0F; //some games won't accept FeliCa cards with a first byte higher than 0x0F  
      Cardio.send(2, uid);
      lastReport = millis();
      cardBusy = 3000;
      // uidLengthMF = 0;
      return;
    }
#endif /* ISO14443 */

	// check for FeliCa card
	nfcFeliCa.reset();
	nfcFeliCa.setupRF();
	uint8_t uidLength = nfcFeliCa.readCardSerial(uid);
	if (uidLength > 0) {
		Serial.println("FeliCa card");
		Cardio.send(2, uid);
		lastReport = millis();
		cardBusy = 3000;
		// uidLength = 0;
		return;
	}

	// check for ISO-15693 card
	nfc15693.reset();
	nfc15693.setupRF();
	// try to read ISO15693 inventory
	ISO15693ErrorCode rc = nfc15693.getInventory(uid);
	if (rc == ISO15693_EC_OK ) {
		Serial.println("ISO15693 card");
		for (int i=0; i<8; i++) {
			hid_data[i] = uid[7-i];
		}
		Cardio.send(1, hid_data);
		lastReport = millis();
		cardBusy = 3000;
		return;
	}
	// no card detected
	Serial.println("No card detected!");
	lastReport = millis();
	cardBusy = 200;
}
