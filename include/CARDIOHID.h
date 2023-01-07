#include "USB.h"
#include "USBHID.h"


static constexpr uint8_t report_descriptor[]={
	0x06, 0xCA, 0xFF,  // Usage Page (Vendor Defined 0xFFCA)
	0x09, 0x01,        // Usage (0x01)
	0xA1, 0x01,        // Collection (Application)
	0x85, 0x01,        //   Report ID (1)
	0x06, 0xCA, 0xFF,  //   Usage Page (Vendor Defined 0xFFCA)
	0x09, 0x41,        //   Usage (0x41)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0xFF,        //   Logical Maximum (-1)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x08,        //   Report Count (8)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x02,        //   Report ID (2)
	0x06, 0xCA, 0xFF,  //   Usage Page (Vendor Defined 0xFFCA)
	0x09, 0x42,        //   Usage (0x42)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0xFF,        //   Logical Maximum (-1)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x08,        //   Report Count (8)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              // End Collection
};

class CARDIOHID_: public USBHIDDevice {
private:
	USBHID HID;
public:
	explicit CARDIOHID_(USBHID hid): HID(hid){
		static bool initialized = false;
		if(!initialized){
			initialized = true;
			USBHID::addDevice(this, sizeof(report_descriptor));
		}
	}

	void begin(){
		HID.begin();
	}

	uint16_t _onGetDescriptor(uint8_t* buffer) override{
		memcpy(buffer, report_descriptor, sizeof(report_descriptor));
		return sizeof(report_descriptor);
	}

	bool send(uint8_t type, uint8_t * uuid){
		if(!HID.ready()) {
			Serial.println("HID not ready!");
		}
		return HID.SendReport(type, uuid, 8);
	}
};

// extern CARDIOHID_ CARDIOHID;
