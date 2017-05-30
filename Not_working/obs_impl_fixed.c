#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include<string.h>
#include <inttypes.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#define MACRO_TAG_NAME "TI BLE Sensor Tag"

#define ATT_CID 				4
#define MAX_SIZE 				200

#define BT_ATT_OP_ERROR_RSP			0x01
#define BT_ATT_OP_MTU_REQ			0x02
#define ATT_OP_MTU_RESP				0x03
#define BT_ATT_OP_FIND_INFO_REQ			0x04
#define BT_ATT_OP_FIND_INFO_RSP			0x05
#define BT_ATT_OP_READ_REQ			0x0A
#define ATT_OP_READ_RESP			0x0B
#define BT_ATT_OP_READ_BY_GRP_TYPE_REQ		0x10
#define ATT_OP_READ_BY_GROUP_RESP		0x11
#define BT_ATT_OP_WRITE_REQ			0x12
#define BT_ATT_OP_WRITE_RSP			0x13


typedef struct
{
    uint16_t uuid;
    const char *str;
} uuid_table_t;
uuid_table_t uuid_table[] = {
    {0x0001, "SDP"},
    {0x0003, "RFCOMM"},
    {0x0005, "TCS-BIN"},
    {0x0007, "ATT"},
    {0x0008, "OBEX"},
    {0x000f, "BNEP"},
    {0x0010, "UPNP"},
    {0x0011, "HIDP"},
    {0x0012, "Hardcopy Control Channel"},
    {0x0014, "Hardcopy Data Channel"},
    {0x0016, "Hardcopy Notification"},
    {0x0017, "AVCTP"},
    {0x0019, "AVDTP"},
    {0x001b, "CMTP"},
    {0x001e, "MCAP Control Channel"},
    {0x001f, "MCAP Data Channel"},
    {0x0100, "L2CAP"},
    /* 0x0101 to 0x0fff undefined */
    {0x1000, "Service Discovery Server Service Class"},
    {0x1001, "Browse Group Descriptor Service Class"},
    {0x1002, "Public Browse Root"},
    /* 0x1003 to 0x1100 undefined */
    {0x1101, "Serial Port"},
    {0x1102, "LAN Access Using PPP"},
    {0x1103, "Dialup Networking"},
    {0x1104, "IrMC Sync"},
    {0x1105, "OBEX Object Push"},
    {0x1106, "OBEX File Transfer"},
    {0x1107, "IrMC Sync Command"},
    {0x1108, "Headset"},
    {0x1109, "Cordless Telephony"},
    {0x110a, "Audio Source"},
    {0x110b, "Audio Sink"},
    {0x110c, "A/V Remote Control Target"},
    {0x110d, "Advanced Audio Distribution"},
    {0x110e, "A/V Remote Control"},
    {0x110f, "A/V Remote Control Controller"},
    {0x1110, "Intercom"},
    {0x1111, "Fax"},
    {0x1112, "Headset AG"},
    {0x1113, "WAP"},
    {0x1114, "WAP Client"},
    {0x1115, "PANU"},
    {0x1116, "NAP"},
    {0x1117, "GN"},
    {0x1118, "Direct Printing"},
    {0x1119, "Reference Printing"},
    {0x111a, "Basic Imaging Profile"},
    {0x111b, "Imaging Responder"},
    {0x111c, "Imaging Automatic Archive"},
    {0x111d, "Imaging Referenced Objects"},
    {0x111e, "Handsfree"},
    {0x111f, "Handsfree Audio Gateway"},
    {0x1120, "Direct Printing Refrence Objects Service"},
    {0x1121, "Reflected UI"},
    {0x1122, "Basic Printing"},
    {0x1123, "Printing Status"},
    {0x1124, "Human Interface Device Service"},
    {0x1125, "Hardcopy Cable Replacement"},
    {0x1126, "HCR Print"},
    {0x1127, "HCR Scan"},
    {0x1128, "Common ISDN Access"},
    /* 0x1129 and 0x112a undefined */
    {0x112d, "SIM Access"},
    {0x112e, "Phonebook Access Client"},
    {0x112f, "Phonebook Access Server"},
    {0x1130, "Phonebook Access"},
    {0x1131, "Headset HS"},
    {0x1132, "Message Access Server"},
    {0x1133, "Message Notification Server"},
    {0x1134, "Message Access Profile"},
    {0x1135, "GNSS"},
    {0x1136, "GNSS Server"},
    {0x1137, "3D Display"},
    {0x1138, "3D Glasses"},
    {0x1139, "3D Synchronization"},
    {0x113a, "MPS Profile"},
    {0x113b, "MPS Service"},
    /* 0x113c to 0x11ff undefined */
    {0x1200, "PnP Information"},
    {0x1201, "Generic Networking"},
    {0x1202, "Generic File Transfer"},
    {0x1203, "Generic Audio"},
    {0x1204, "Generic Telephony"},
    {0x1205, "UPNP Service"},
    {0x1206, "UPNP IP Service"},
    {0x1300, "UPNP IP PAN"},
    {0x1301, "UPNP IP LAP"},
    {0x1302, "UPNP IP L2CAP"},
    {0x1303, "Video Source"},
    {0x1304, "Video Sink"},
    {0x1305, "Video Distribution"},
    /* 0x1306 to 0x13ff undefined */
    {0x1400, "HDP"},
    {0x1401, "HDP Source"},
    {0x1402, "HDP Sink"},
    /* 0x1403 to 0x17ff undefined */
    {0x1800, "Generic Access Profile"},
    {0x1801, "Generic Attribute Profile"},
    {0x1802, "Immediate Alert"},
    {0x1803, "Link Loss"},
    {0x1804, "Tx Power"},
    {0x1805, "Current Time Service"},
    {0x1806, "Reference Time Update Service"},
    {0x1807, "Next DST Change Service"},
    {0x1808, "Glucose"},
    {0x1809, "Health Thermometer"},
    {0x180a, "Device Information"},
    /* 0x180b and 0x180c undefined */
    {0x180d, "Heart Rate"},
    {0x180e, "Phone Alert Status Service"},
    {0x180f, "Battery Service"},
    {0x1810, "Blood Pressure"},
    {0x1811, "Alert Notification Service"},
    {0x1812, "Human Interface Device"},
    {0x1813, "Scan Parameters"},
    {0x1814, "Running Speed and Cadence"},
    {0x1815, "Automation IO"},
    {0x1816, "Cycling Speed and Cadence"},
    /* 0x1817 undefined */
    {0x1818, "Cycling Power"},
    {0x1819, "Location and Navigation"},
    {0x181a, "Environmental Sensing"},
    {0x181b, "Body Composition"},
    {0x181c, "User Data"},
    {0x181d, "Weight Scale"},
    {0x181e, "Bond Management"},
    {0x181f, "Continuous Glucose Monitoring"},
    {0x1820, "Internet Protocol Support"},
    {0x1821, "Indoor Positioning"},
    {0x1822, "Pulse Oximeter"},
    {0x1823, "HTTP Proxy"},
    {0x1824, "Transport Discovery"},
    {0x1825, "Object Transfer"},
    /* 0x1824 to 0x27ff undefined */
    {0x2800, "Primary Service"},
    {0x2801, "Secondary Service"},
    {0x2802, "Include"},
    {0x2803, "Characteristic"},
    /* 0x2804 to 0x28ff undefined */
    {0x2900, "Characteristic Extended Properties"},
    {0x2901, "Characteristic User Description"},
    {0x2902, "Client Characteristic Configuration"},
    {0x2903, "Server Characteristic Configuration"},
    {0x2904, "Characteristic Format"},
    {0x2905, "Characteristic Aggregate Formate"},
    {0x2906, "Valid Range"},
    {0x2907, "External Report Reference"},
    {0x2908, "Report Reference"},
    {0x2909, "Number of Digitals"},
    {0x290a, "Value Trigger Setting"},
    {0x290b, "Environmental Sensing Configuration"},
    {0x290c, "Environmental Sensing Measurement"},
    {0x290d, "Environmental Sensing Trigger Setting"},
    {0x290e, "Time Trigger Setting"},
    /* 0x290f to 0x29ff undefined */
    {0x2a00, "Device Name"},
    {0x2a01, "Appearance"},
    {0x2a02, "Peripheral Privacy Flag"},
    {0x2a03, "Reconnection Address"},
    {0x2a04, "Peripheral Preferred Connection Parameters"},
    {0x2a05, "Service Changed"},
    {0x2a06, "Alert Level"},
    {0x2a07, "Tx Power Level"},
    {0x2a08, "Date Time"},
    {0x2a09, "Day of Week"},
    {0x2a0a, "Day Date Time"},
    /* 0x2a0b undefined */
    {0x2a0c, "Exact Time 256"},
    {0x2a0d, "DST Offset"},
    {0x2a0e, "Time Zone"},
    {0x2a0f, "Local Time Information"},
    /* 0x2a10 undefined */
    {0x2a11, "Time with DST"},
    {0x2a12, "Time Accuracy"},
    {0x2a13, "Time Source"},
    {0x2a14, "Reference Time Information"},
    /* 0x2a15 undefined */
    {0x2a16, "Time Update Control Point"},
    {0x2a17, "Time Update State"},
    {0x2a18, "Glucose Measurement"},
    {0x2a19, "Battery Level"},
    /* 0x2a1a and 0x2a1b undefined */
    {0x2a1c, "Temperature Measurement"},
    {0x2a1d, "Temperature Type"},
    {0x2a1e, "Intermediate Temperature"},
    /* 0x2a1f and 0x2a20 undefined */
    {0x2a21, "Measurement Interval"},
    {0x2a22, "Boot Keyboard Input Report"},
    {0x2a23, "System ID"},
    {0x2a24, "Model Number String"},
    {0x2a25, "Serial Number String"},
    {0x2a26, "Firmware Revision String"},
    {0x2a27, "Hardware Revision String"},
    {0x2a28, "Software Revision String"},
    {0x2a29, "Manufacturer Name String"},
    {0x2a2a, "IEEE 11073-20601 Regulatory Cert. Data List"},
    {0x2a2b, "Current Time"},
    {0x2a2c, "Magnetic Declination"},
    /* 0x2a2d to 0x2a30 undefined */
    {0x2a31, "Scan Refresh"},
    {0x2a32, "Boot Keyboard Output Report"},
    {0x2a33, "Boot Mouse Input Report"},
    {0x2a34, "Glucose Measurement Context"},
    {0x2a35, "Blood Pressure Measurement"},
    {0x2a36, "Intermediate Cuff Pressure"},
    {0x2a37, "Heart Rate Measurement"},
    {0x2a38, "Body Sensor Location"},
    {0x2a39, "Heart Rate Control Point"},
    /* 0x2a3a to 0x2a3e undefined */
    {0x2a3f, "Alert Status"},
    {0x2a40, "Ringer Control Point"},
    {0x2a41, "Ringer Setting"},
    {0x2a42, "Alert Category ID Bit Mask"},
    {0x2a43, "Alert Category ID"},
    {0x2a44, "Alert Notification Control Point"},
    {0x2a45, "Unread Alert Status"},
    {0x2a46, "New Alert"},
    {0x2a47, "Supported New Alert Category"},
    {0x2a48, "Supported Unread Alert Category"},
    {0x2a49, "Blood Pressure Feature"},
    {0x2a4a, "HID Information"},
    {0x2a4b, "Report Map"},
    {0x2a4c, "HID Control Point"},
    {0x2a4d, "Report"},
    {0x2a4e, "Protocol Mode"},
    {0x2a4f, "Scan Interval Window"},
    {0x2a50, "PnP ID"},
    {0x2a51, "Glucose Feature"},
    {0x2a52, "Record Access Control Point"},
    {0x2a53, "RSC Measurement"},
    {0x2a54, "RSC Feature"},
    {0x2a55, "SC Control Point"},
    {0x2a56, "Digital"},
    /* 0x2a57 undefined */
    {0x2a58, "Analog"},
    /* 0x2a59 undefined */
    {0x2a5a, "Aggregate"},
    {0x2a5b, "CSC Measurement"},
    {0x2a5c, "CSC Feature"},
    {0x2a5d, "Sensor Location"},
    /* 0x2a5e to 0x2a62 undefined */
    {0x2a63, "Cycling Power Measurement"},
    {0x2a64, "Cycling Power Vector"},
    {0x2a65, "Cycling Power Feature"},
    {0x2a66, "Cycling Power Control Point"},
    {0x2a67, "Location and Speed"},
    {0x2a68, "Navigation"},
    {0x2a69, "Position Quality"},
    {0x2a6a, "LN Feature"},
    {0x2a6b, "LN Control Point"},
    {0x2a6c, "Elevation"},
    {0x2a6d, "Pressure"},
    {0x2a6e, "Temperature"},
    {0x2a6f, "Humidity"},
    {0x2a70, "True Wind Speed"},
    {0x2a71, "True Wind Direction"},
    {0x2a72, "Apparent Wind Speed"},
    {0x2a73, "Apparent Wind Direction"},
    {0x2a74, "Gust Factor"},
    {0x2a75, "Pollen Concentration"},
    {0x2a76, "UV Index"},
    {0x2a77, "Irradiance"},
    {0x2a78, "Rainfall"},
    {0x2a79, "Wind Chill"},
    {0x2a7a, "Heat Index"},
    {0x2a7b, "Dew Point"},
    {0x2a7c, "Trend"},
    {0x2a7d, "Descriptor Value Changed"},
    {0x2a7e, "Aerobic Heart Rate Lower Limit"},
    {0x2a7f, "Aerobic Threshold"},
    {0x2a80, "Age"},
    {0x2a81, "Anaerobic Heart Rate Lower Limit"},
    {0x2a82, "Anaerobic Heart Rate Upper Limit"},
    {0x2a83, "Anaerobic Threshold"},
    {0x2a84, "Aerobic Heart Rate Upper Limit"},
    {0x2a85, "Date of Birth"},
    {0x2a86, "Date of Threshold Assessment"},
    {0x2a87, "Email Address"},
    {0x2a88, "Fat Burn Heart Rate Lower Limit"},
    {0x2a89, "Fat Burn Heart Rate Upper Limit"},
    {0x2a8a, "First Name"},
    {0x2a8b, "Five Zone Heart Rate Limits"},
    {0x2a8c, "Gender"},
    {0x2a8d, "Heart Rate Max"},
    {0x2a8e, "Height"},
    {0x2a8f, "Hip Circumference"},
    {0x2a90, "Last Name"},
    {0x2a91, "Maximum Recommended Heart Rate"},
    {0x2a92, "Resting Heart Rate"},
    {0x2a93, "Sport Type for Aerobic/Anaerobic Thresholds"},
    {0x2a94, "Three Zone Heart Rate Limits"},
    {0x2a95, "Two Zone Heart Rate Limit"},
    {0x2a96, "VO2 Max"},
    {0x2a97, "Waist Circumference"},
    {0x2a98, "Weight"},
    {0x2a99, "Database Change Increment"},
    {0x2a9a, "User Index"},
    {0x2a9b, "Body Composition Feature"},
    {0x2a9c, "Body Composition Measurement"},
    {0x2a9d, "Weight Measurement"},
    {0x2a9e, "Weight Scale Feature"},
    {0x2a9f, "User Control Point"},
    {0x2aa0, "Magnetic Flux Density - 2D"},
    {0x2aa1, "Magnetic Flux Density - 3D"},
    {0x2aa2, "Language"},
    {0x2aa3, "Barometric Pressure Trend"},
    {0x2aa4, "Bond Management Control Point"},
    {0x2aa5, "Bond Management Feature"},
    {0x2aa6, "Central Address Resolution"},
    {0x2aa7, "CGM Measurement"},
    {0x2aa8, "CGM Feature"},
    {0x2aa9, "CGM Status"},
    {0x2aaa, "CGM Session Start Time"},
    {0x2aab, "CGM Session Run Time"},
    {0x2aac, "CGM Specific Ops Control Point"},
    {0x2aad, "Indoor Positioning Configuration"},
    {0x2aae, "Latitude"},
    {0x2aaf, "Longitude"},
    {0x2ab0, "Local North Coordinate"},
    {0x2ab1, "Local East Coordinate"},
    {0x2ab2, "Floor Number"},
    {0x2ab3, "Altitude"},
    {0x2ab4, "Uncertainty"},
    {0x2ab5, "Location Name"},
    {0x2ab6, "URI"},
    {0x2ab7, "HTTP Headers"},
    {0x2ab8, "HTTP Status Code"},
    {0x2ab9, "HTTP Entity Body"},
    {0x2aba, "HTTP Control Point"},
    {0x2abb, "HTTPS Security"},
    {0x2abc, "TDS Control Point"},
    {0x2abd, "OTS Feature"},
    {0x2abe, "Object Name"},
    {0x2abf, "Object Type"},
    {0x2ac0, "Object Size"},
    {0x2ac1, "Object First-Created"},
    {0x2ac2, "Object Last-Modified"},
    {0x2ac3, "Object ID"},
    {0x2ac4, "Object Properties"},
    {0x2ac5, "Object Action Control Point"},
    {0x2ac6, "Object List Control Point"},
    {0x2ac7, "Object List Filter"},
    {0x2ac8, "Object Changed"},
    /* vendor defined */
    {0xfeff, "GN Netcom"},
    {0xfefe, "GN ReSound A/S"},
    {0xfefd, "Gimbal, Inc."},
    {0xfefc, "Gimbal, Inc."},
    {0xfefb, "Stollmann E+V GmbH"},
    {0xfefa, "PayPal, Inc."},
    {0xfef9, "PayPal, Inc."},
    {0xfef8, "Aplix Corporation"},
    {0xfef7, "Aplix Corporation"},
    {0xfef6, "Wicentric, Inc."},
    {0xfef5, "Dialog Semiconductor GmbH"},
    {0xfef4, "Google"},
    {0xfef3, "Google"},
    {0xfef2, "CSR"},
    {0xfef1, "CSR"},
    {0xfef0, "Intel"},
    {0xfeef, "Polar Electro Oy"},
    {0xfeee, "Polar Electro Oy"},
    {0xfeed, "Tile, Inc."},
    {0xfeec, "Tile, Inc."},
    {0xfeeb, "Swirl Networks, Inc."},
    {0xfeea, "Swirl Networks, Inc."},
    {0xfee9, "Quintic Corp."},
    {0xfee8, "Quintic Corp."},
    {0xfee7, "Tencent Holdings Limited"},
    {0xfee6, "Seed Labs, Inc."},
    {0xfee5, "Nordic Semiconductor ASA"},
    {0xfee4, "Nordic Semiconductor ASA"},
    {0xfee3, "Anki, Inc."},
    {0xfee2, "Anki, Inc."},
    {0xfee1, "Anhui Huami Information Technology Co."},
    {0xfee0, "Anhui Huami Information Technology Co."},
    {0xfedf, "Design SHIFT"},
    {0xfede, "Coin, Inc."},
    {0xfedd, "Jawbone"},
    {0xfedc, "Jawbone"},
    {0xfedb, "Perka, Inc."},
    {0xfeda, "ISSC Technologies Corporation"},
    {0xfed9, "Pebble Technology Corporation"},
    {0xfed8, "Google"},
    {0xfed7, "Broadcom Corporation"},
    {0xfed6, "Broadcom Corporation"},
    {0xfed5, "Plantronics Inc."},
    {0xfed4, "Apple, Inc."},
    {0xfed3, "Apple, Inc."},
    {0xfed2, "Apple, Inc."},
    {0xfed1, "Apple, Inc."},
    {0xfed0, "Apple, Inc."},
    {0xfecf, "Apple, Inc."},
    {0xfece, "Apple, Inc."},
    {0xfecd, "Apple, Inc."},
    {0xfecc, "Apple, Inc."},
    {0xfecb, "Apple, Inc."},
    {0xfeca, "Apple, Inc."},
    {0xfec9, "Apple, Inc."},
    {0xfec8, "Apple, Inc."},
    {0xfec7, "Apple, Inc."},
    {0xfec6, "Kocomojo, LLC"},
    {0xfec5, "Realtek Semiconductor Corp."},
    {0xfec4, "PLUS Location Systems"},
    {0xfec3, "360fly, Inc."},
    {0xfec2, "Blue Spark Technologies, Inc."},
    {0xfec1, "KDDI Corporation"},
    {0xfec0, "KDDI Corporation"},
    {0xfebf, "Nod, Inc."},
    {0xfebe, "Bose Corporation"},
    {0xfebd, "Clover Network, Inc."},
    {0xfebc, "Dexcom, Inc."},
    {0xfebb, "adafruit industries"},
    {0xfeba, "Tencent Holdings Limited"},
    {0xfeb9, "LG Electronics"},
    {0xfeb8, "Facebook, Inc."},
    {0xfeb7, "Facebook, Inc."},
    {0xfeb6, "Vencer Co, Ltd"},
    {0xfeb5, "WiSilica Inc."},
    {0xfeb4, "WiSilica Inc."},
    {0xfeb3, "Taobao"},
    {0xfeb2, "Microsoft Corporation"},
    {0xfeb1, "Electronics Tomorrow Limited"},
    {0xfeb0, "Nest Labs Inc."},
    {0xfeaf, "Nest Labs Inc."},
    {0xfeae, "Nokia Corporation"},
    {0xfead, "Nokia Corporation"},
    {0xfeac, "Nokia Corporation"},
    {0xfeab, "Nokia Corporation"},
    {0xfeaa, "Google"},
    {0xfea9, "Savant Systems LLC"},
    {0xfea8, "Savant Systems LLC"},
    {0xfea7, "UTC Fire and Security"},
    {0xfea6, "GoPro, Inc."},
    {0xfea5, "GoPro, Inc."},
    {0xfea4, "Paxton Access Ltd"},
    {0xfea3, "ITT Industries"},
    {0xfea2, "Intrepid Control Systems, Inc."},
    {0xfea1, "Intrepid Control Systems, Inc."},
    {0xfea0, "Google"},
    {0xfe9f, "Google"},
    {0xfe9e, "Dialog Semiconductor B.V."},
    {0xfe9d, "Mobiquity Networks Inc"},
    {0xfe9c, "GSI Laboratories, Inc."},
    {0xfe9b, "Samsara Networks, Inc"},
    {0xfe9a, "Estimote"},
    {0xfe99, "Currant, Inc."},
    {0xfe98, "Currant, Inc."},
    {0xfe97, "Tesla Motor Inc."},
    {0xfe96, "Tesla Motor Inc."},
    {0xfe95, "Xiaomi Inc."},
    {0xfe94, "OttoQ Inc."},
    {0xfe93, "OttoQ Inc."},
    {0xfe92, "Jarden Safety & Security"},
    {0xfe91, "Shanghai Imilab Technology Co.,Ltd"},
    {0xfe90, "JUMA"},
    {0xfe8f, "CSR"},
    {0xfe8e, "ARM Ltd"},
    {0xfe8d, "Interaxon Inc."},
    {0xfe8c, "TRON Forum"},
    {0xfe8b, "Apple, Inc."},
    {0xfe8a, "Apple, Inc."},
    {0xfe89, "B&O Play A/S"},
    {0xfe88, "SALTO SYSTEMS S.L."},
    {0xfe87,
     "Qingdao Yeelink Information Technology Co., Ltd. ( 青岛亿联客信息技术有限公司 )"},
    {0xfe86, "HUAWEI Technologies Co., Ltd. ( 华为技术有限公司 )"},
    {0xfe85, "RF Digital Corp"},
    {0xfe84, "RF Digital Corp"},
    {0xfe83, "Blue Bite"},
    {0xfe82, "Medtronic Inc."},
    {0xfe81, "Medtronic Inc."},
    {0xfe80, "Doppler Lab"},
    {0xfe7f, "Doppler Lab"},
    {0xfe7e, "Awear Solutions Ltd"},
    {0xfe7d, "Aterica Health Inc."},
    {0xfe7c, "Stollmann E+V GmbH"},
    {0xfe7b, "Orion Labs, Inc."},
    /* SDO defined */
    {0xfffe, "Alliance for Wireless Power (A4WP)"},
    {0xfffd, "Fast IDentity Online Alliance (FIDO)"},
    /*TI sensor Tag defined */
    {0xAA01, "IR Temperature Data"},
    {0xAA02, "IR Temperature Config"},
    {0xAA03, "IR Temperature Period"},
    {0xAA11, "Accelerometer Data"},
    {0xAA12, "Accelerometer Config"},
    {0xAA13, "Accelerometer Period"},
    {0xAA21, "Humidity Data"},
    {0xAA22, "Humidity Config"},
    {0xAA23, "Humidity Period"},
    {0xAA31, "Magnetometer Data"},
    {0xAA32, "Magnetometer Config"},
    {0xAA33, "Magnetometer Period"},
    {0xAA41, "Barometer Data"},
    {0xAA42, "Barometer Configuration"},
    {0xAA44, "Barometer period"},
    {0xAA43, "Barometer Calibration"},
    {0xAA51, "Gyroscope Data"},
    {0xAA52, "Gyroscope Config"},
    {0xAA53, "Gyroscope Period"},
    {0xAA61, "Test Data"},
    {0xAA62, "Test Config"},
    {0xCCC1, "Connection Parameters"},
    {0xCCC2, "Request Connection Parameters"},
    {0xCCC3, "Disconnect request"},
    {0xFFC1, "OAD Image Identify"},
    {0xFFC2, "OAD Image Block"},
    {0xFFE1, "simple key service"},
    {}
};

//unsigned char g_address[18];
unsigned char global_temp[512];
struct observe_struct
{
    unsigned char key[28];//MQTT request
    unsigned char value[512];//afterc comparison
    uint16_t handle; //after comaparison
};


typedef struct 
{ 
    unsigned char mac_id[50];//MQTT request
    uint16_t num_of_keys;
    struct observe_struct observe_pair[10];
}OBSERVE_T;

OBSERVE_T   obs_table[MAX_SIZE] = { 0 };
static int obs_table_index =0;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct key_value_pair
{
    unsigned char key[28];
    unsigned char value[512];
    uint16_t handle_res;
    struct key_value_pair *pNext;
} key_value_pair_t;   

key_value_pair_t *pKeyValue_Global = NULL;

typedef struct information
{
    unsigned char type[16];
    unsigned char value[512];
    uint16_t handle;
    uint16_t type_uuid;//1 or 16 bit uuid //2 for 128 bit uuid
} info;
info characteristic_table[MAX_SIZE] = { 0 };
static int table_index = 0;
char *uuid_to_str (uint16_t uuid)
{
    int i;
    for (i = 0; i < (sizeof (uuid_table) / sizeof (uuid_table_t)); i++)
    {
        if (uuid_table[i].uuid == uuid)
            return uuid_table[i].str;
    }

    return "Unknown";
}

int get_table_index_by_uuid (uint16_t hnd, int index_val)
{
    int i;
    if(index_val < table_index)
    {
        for (i = index_val; i < table_index; i++)
        {
            if ((characteristic_table[i].type[12] == (hnd & 0xff)) && (characteristic_table[i].type[13] == ((hnd >> 8) & 0xff)))
            {
                return i;
            }
        }
    }
    return 0xffff;
}

int get_handle_by_index (uint16_t index)
{
    int i;
    for (i = 0; i < table_index; i++)
    {
        if (i == index)
            return characteristic_table[i].handle;
    }
    return 0xffff;
}

int get_index_by_handle (uint16_t hnd)
{
    int i;
    for (i = 0; i < table_index; i++)
    {
        if (characteristic_table[i].handle == hnd)
            return i;
    }
    return 0xffff;
}

key_value_pair_t *form_key_value_pair (int fd,info * tbl)
{
    int index = 0;
    int i;
    int primary_index;
    int nCharIndex;
    int key_value_pair_index;
    key_value_pair_t *pKeyValue_LocalNode = NULL;
    key_value_pair_t *pKeyValue_Temp = NULL;
    //key_value_pair_t *pKeyValue_local = NULL;
    unsigned char *pcKey;
    uint16_t handle_result,attr_handle;
    uint16_t test_uuid;
    unsigned char wpdu[512];
    unsigned char rpdu[512];
    unsigned int len = 0;
    unsigned int size = 0;
    while(pKeyValue_Global != NULL) {
        pKeyValue_Temp = pKeyValue_Global;
        pKeyValue_Global = pKeyValue_Global->pNext;
        free(pKeyValue_Temp);
    }
    pKeyValue_Temp = NULL;
    while ((primary_index = get_table_index_by_uuid (0x2800, index)) != 0xffff)
    {
        nCharIndex = get_table_index_by_uuid (0x2803, primary_index);
        attr_handle = get_handle_by_index(nCharIndex) + 1 ;
        //printf("from_handle: %.4x \n", attr_handle);
        key_value_pair_index = get_index_by_handle (attr_handle);
        memset(wpdu, '\0', 512);
        memset(rpdu, '\0', 512);
        wpdu[0] = BT_ATT_OP_READ_REQ;
        wpdu[1] = attr_handle & 0xff;
        wpdu[2] = (attr_handle & 0xff00) >> 8;
        size = write (fd, wpdu, 3);
        len = read (fd, rpdu, 512);
        //printf("%.2x\n",rpdu[0]);
        //      if (rpdu[0] == 0x01)
        //        {
        //          memset(rpdu,'\0',512);
        //          //printf("nnnnnn------\n");
        //          size = write (fd, wpdu, 3);
        //          len =read(fd,rpdu,512);
        //      }
        //      if (rpdu[0] == 0x1b)
        //      {
        //          memset(rpdu,'\0',512);
        //           //printf("nnnnnn------\n");
        //            size = write (fd, wpdu, 3);
        //          len = read(fd,rpdu,512);
        //      }

        //      printf("len in form_key_value_pair %d rpdu[0] %.2x\n",len,rpdu[0]);
        //      for(int g=0; g<len; g++)
        //      {
        //          printf("rpdu[%d]= %.2x \n",g,rpdu[g]);
        //      }
        memset (tbl[key_value_pair_index].value, 0, sizeof (tbl[key_value_pair_index].value));
        memcpy(tbl[key_value_pair_index].value,rpdu,len);

        if(key_value_pair_index == 0xffff)
        {
            printf("index not found\n");
            index = nCharIndex;
            continue;
        }
        pKeyValue_LocalNode = malloc (sizeof (key_value_pair_t));
        pKeyValue_LocalNode->pNext = NULL;
        test_uuid = tbl[key_value_pair_index].type[13] << 8 | tbl[key_value_pair_index].type[12];
        pcKey = uuid_to_str (test_uuid);
        strcpy (pKeyValue_LocalNode->key, pcKey);
        memcpy (pKeyValue_LocalNode->value, tbl[key_value_pair_index].value, 512);
        pKeyValue_LocalNode->handle_res = tbl[key_value_pair_index].handle;
        /*pKeyValue_Temp = pKeyValue_local;

      if (pKeyValue_local == NULL)
        {
          pKeyValue_local = pKeyValue_LocalNode;
        }*/
        pKeyValue_Temp = pKeyValue_Global;
        if (pKeyValue_Global == NULL)
        {
            pKeyValue_Global = pKeyValue_LocalNode;
        }
        else
        {
            while (pKeyValue_Temp->pNext != NULL)
            {
                pKeyValue_Temp = pKeyValue_Temp->pNext;
            }
            pKeyValue_Temp->pNext = pKeyValue_LocalNode;
        }
        index = nCharIndex;
    }
    // return pKeyValue_local;
    return pKeyValue_Global;
}
key_value_pair_t * Write_data(int fd,info * tbl)    //To Put
{
    unsigned char wpdu[512];
    unsigned char rpdu[512];
    unsigned int len = 0;
    unsigned int i = 0, j = 0;
    unsigned int size = 0;
    unsigned char *pcKey;
    uint16_t key_value_pair_index;
    uint16_t test_uuid;
    uint16_t temp_id;
    uint16_t user_id;
    key_value_pair_t *pKeyValue_LocalNode1 = NULL;
    key_value_pair_t *pKeyValue_Temp = NULL;
    //key_value_pair_t *pKeyValue_local = NULL;
    len = 0;
    for(i=0;i<table_index;i++)
    {
        if(characteristic_table[i].type_uuid == 0x02)
        {

            temp_id = (characteristic_table[i].type[13]<< 8) | characteristic_table[i].type[12] ;
            //printf("temp_id: %.4x\n",temp_id);
            //            printf("Give input as '0xaa02' for IR Temperature Config\n\t'0xaa12' for Accelerometer Config\n\t'0xaa22' for Humidity Config\n\t'0xaa32' for Magnetometer Config\n\t'0xaa42' for Barometer Configuration\n\t'0xaa52' for Gyroscope Config\n\t '0xaa62' for Test Config:\n");
            //            scanf("%d", &user_id);
            //            printf("user_id: %d\n",user_id);
            //if(temp_id == 0xaa02 ||  temp_id == 0xaa12 || temp_id == 0xaa22 || temp_id == 0xaa32 || temp_id == 0xaa42 || temp_id == 0xaa52 || temp_id == 0xaa62)
            if(temp_id ==  0xaa12)
            {
                printf("============compare================\n");
                key_value_pair_index = get_index_by_handle(characteristic_table[i].handle);
                printf("write_data handle: %.4x \n", characteristic_table[i].handle);
                memset(wpdu, '\0', 512);
                memset(rpdu, '\0', 512);
                wpdu[0] = BT_ATT_OP_WRITE_REQ;
                wpdu[1] = characteristic_table[i].handle & 0xff;
                wpdu[2] = (characteristic_table[i].handle & 0xff00) >> 8;
                wpdu[3] = 0x01;// only one byte writing

                size = write (fd, wpdu, 4);
                len = read (fd, rpdu, 512);
                //                printf("len in write_data %d rpdu[0] %.2x\n",len,rpdu[0]);
                //                for(int g=0; g<len; g++)
                //                {
                //                    printf("rpdu[%d]= %.2x \n",g,rpdu[g]);
                //                }

                memset (tbl[key_value_pair_index].value, 0, sizeof (tbl[key_value_pair_index].value));
                memcpy(tbl[key_value_pair_index].value,rpdu,len);

                if(key_value_pair_index == 0xffff)
                {
                    printf("index not found\n");
                    break;
                }
                pKeyValue_LocalNode1 = malloc (sizeof (key_value_pair_t));
                pKeyValue_LocalNode1->pNext = NULL;
                test_uuid = tbl[key_value_pair_index].type[13] << 8 | tbl[key_value_pair_index].type[12];
                pcKey = uuid_to_str (test_uuid);
                strcpy (pKeyValue_LocalNode1->key, pcKey);
                memcpy (pKeyValue_LocalNode1->value, tbl[key_value_pair_index].value, 512);
                int m=0;
                unsigned char temp[512];
                memset(temp, '\0',512);
                for (int n=0; n<20;n++)
                {
                    sprintf(&temp[m],"%.2X ",tbl[key_value_pair_index].value[n]);
                    m=m+3;
                }
                printf("key : %s value : %s", pcKey,temp );
                pKeyValue_LocalNode1->handle_res = tbl[key_value_pair_index].handle;
                /* pKeyValue_Temp = pKeyValue_local;
                if (pKeyValue_local == NULL)
                  {
                    pKeyValue_local = pKeyValue_LocalNode1;
                  }*/
                pKeyValue_Temp = pKeyValue_Global;
                if (pKeyValue_Global == NULL)
                {
                    pKeyValue_Global = pKeyValue_LocalNode1;
                }
                else
                {
                    while (pKeyValue_Temp->pNext != NULL)
                    {
                        pKeyValue_Temp = pKeyValue_Temp->pNext;
                    }
                    pKeyValue_Temp->pNext = pKeyValue_LocalNode1;
                }
            }

        }
    }
    return pKeyValue_Global;
}

key_value_pair_t * Write_to_pair(int fd,info * tbl)
{
    unsigned char wpdu[512];
    unsigned char rpdu[512];
    unsigned int len = 0;
    unsigned int i = 0, j = 0;
    unsigned int size = 0;
    unsigned char *pcKey;
    uint16_t key_value_pair_index;
    uint16_t test_uuid;
    uint16_t temp_id;
    key_value_pair_t *pKeyValue_LocalNode2 = NULL;
    key_value_pair_t *pKeyValue_Temp = NULL;
    // key_value_pair_t *pKeyValue_local = NULL;
    len = 0;
    for(i=0;i<table_index;i++)
    {
        if(characteristic_table[i].type_uuid == 0x02)
        {

            temp_id = (characteristic_table[i].type[13]<< 8) | characteristic_table[i].type[12] ;
            //if((temp_id & 0x000f) ==  0x0002)
            if(temp_id == 0xaa02 ||  temp_id == 0xaa12 || temp_id == 0xaa22 || temp_id == 0xaa32 || temp_id == 0xaa42 || temp_id == 0xaa52 || temp_id == 0xaa62)
            {
                key_value_pair_index = get_index_by_handle(characteristic_table[i].handle);
                //printf("Write_to_pair handle: %.4x \n", characteristic_table[i].handle);
                memset(wpdu, '\0', 512);
                memset(rpdu, '\0', 512);
                wpdu[0] = BT_ATT_OP_READ_REQ;
                wpdu[1] = characteristic_table[i].handle & 0xff;
                wpdu[2] = (characteristic_table[i].handle & 0xff00) >> 8;

                size = write (fd, wpdu, 3);
                len = read (fd, rpdu, 512);
                //                printf("len in write_to_pair %d rpdu[0] %.2x\n",len,rpdu[0]);
                //                for(int g=0; g<len; g++)
                //                {
                //                    printf("rpdu[%d]= %.2x \n",g,rpdu[g]);
                //                }

                memset (tbl[key_value_pair_index].value, 0, sizeof (tbl[key_value_pair_index].value));
                memcpy(tbl[key_value_pair_index].value,rpdu,len);

                if(key_value_pair_index == 0xffff)
                {
                    printf("index not found\n");
                    break;
                }
                pKeyValue_LocalNode2 = malloc (sizeof (key_value_pair_t));
                pKeyValue_LocalNode2->pNext = NULL;
                test_uuid = tbl[key_value_pair_index].type[13] << 8 | tbl[key_value_pair_index].type[12];
                pcKey = uuid_to_str (test_uuid);
                strcpy (pKeyValue_LocalNode2->key, pcKey);
                memcpy (pKeyValue_LocalNode2->value, tbl[key_value_pair_index].value, 512);
                pKeyValue_LocalNode2->handle_res = tbl[key_value_pair_index].handle;
                /*pKeyValue_Temp = pKeyValue_local;
                if (pKeyValue_local == NULL)
                  {
                    pKeyValue_local = pKeyValue_LocalNode2;
                  }*/
                pKeyValue_Temp = pKeyValue_Global;
                if (pKeyValue_Global == NULL)
                {
                    pKeyValue_Global = pKeyValue_LocalNode2;
                }
                else
                {
                    while (pKeyValue_Temp->pNext != NULL)
                    {
                        pKeyValue_Temp = pKeyValue_Temp->pNext;
                    }
                    pKeyValue_Temp->pNext = pKeyValue_LocalNode2;
                }
            }
        }
    }
    //return pKeyValue_local;
    return pKeyValue_Global;
}

void write_to_table (int fd, char start[], char stop[])
{
    unsigned char wpdu[512];
    unsigned char rpdu[512];
    unsigned int len = 0;
    unsigned int i = 0, j = 0;
    unsigned int size = 0;
    //int temp_index = 0;
    uint16_t last_handle;
    wpdu[0] = BT_ATT_OP_FIND_INFO_REQ;
    wpdu[1] = start[1];
    wpdu[2] = start[0];
    wpdu[3] = stop[1];
    wpdu[4] = stop[0];
    last_handle = start[1] | (start[0] << 8);
    len = 0;
    while (last_handle != 0xffff)
    {
        wpdu[1] = last_handle & 0xff;
        wpdu[2] = (last_handle & 0xff00) >> 8;
        size = write (fd, wpdu, 5);
        len = read (fd, rpdu, 512);
        if (rpdu[0] != BT_ATT_OP_ERROR_RSP)
        {

            if (rpdu[1] == 0x01)
            {
                for (i = 0; i < ((len - 2) / 4); i++)
                {
                    j = (4 * i) + 2;
                    characteristic_table[table_index].handle = (rpdu[j + 1] << 8) | rpdu[j + 0];
                    characteristic_table[table_index].type[13] = rpdu[j + 3];
                    characteristic_table[table_index].type[12] = rpdu[j + 2];
                    characteristic_table[table_index].type_uuid = 0x01;
                    last_handle = characteristic_table[table_index].handle + 1;
                    table_index++;
                }
            }
            else if (rpdu[1] == 0x02)
            {
                for (i = 0; i < ((len - 2) / 18); i++)
                {
                    j = (i * 18) + 2;
                    characteristic_table[table_index].handle = rpdu[j + 1] << 8 | rpdu[j + 0];
                    memcpy (characteristic_table[table_index].type,&rpdu[j + 2], 16);
                    characteristic_table[table_index].type_uuid = 0x02;
                    last_handle = characteristic_table[table_index].handle + 1;
                    table_index++;
                }
            }
        }
        else
        {
            return;
        }

    }
    return;
}

static bool verbose = false;
static int l2cap_le_att_connect (bdaddr_t * src, bdaddr_t * dst, uint8_t dst_type, int sec)
{
    int sock;
    struct sockaddr_l2 srcaddr, dstaddr;
    struct bt_security btsec;
    if (verbose)
    {
        char srcaddr_str[18], dstaddr_str[18];
        ba2str (src, srcaddr_str);
        ba2str (dst, dstaddr_str);
        printf ("btgatt-client: Opening L2CAP LE connection on ATT \n channel:\n\t src: %s\n\tdest: %s\n", srcaddr_str, dstaddr_str);
    }

    sock = socket (PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (sock < 0)
    {
        perror ("Failed to create L2CAP socket");
        return -1;
    }

    /* Set up source address */
    memset (&srcaddr, 0, sizeof (srcaddr));
    srcaddr.l2_family = AF_BLUETOOTH;
    srcaddr.l2_cid = htobs (ATT_CID);
    srcaddr.l2_bdaddr_type = 0;
    bacpy (&srcaddr.l2_bdaddr, src);

    if (bind (sock, (struct sockaddr *) &srcaddr, sizeof (srcaddr)) < 0)
    {
        perror ("Failed to bind L2CAP socket");
        close (sock);
        return -1;
    }

    /* Set the security level */
    memset (&btsec, 0, sizeof (btsec));
    btsec.level = sec;
    if (setsockopt (sock, SOL_BLUETOOTH, BT_SECURITY, &btsec,
                    sizeof (btsec)) != 0)
    {
        fprintf (stderr, "Failed to set L2CAP security level\n");
        close (sock);
        return -1;
    }

    /* Set up destination address */
    memset (&dstaddr, 0, sizeof (dstaddr));
    dstaddr.l2_family = AF_BLUETOOTH;
    dstaddr.l2_cid = htobs (ATT_CID);
    dstaddr.l2_bdaddr_type = dst_type;
    bacpy (&dstaddr.l2_bdaddr, dst);

    printf ("Connecting to device...");
    fflush (stdout);

    if (connect (sock, (struct sockaddr *) &dstaddr, sizeof (dstaddr)) < 0)
    {
        perror (" Failed to connect");
        close (sock);
        return -1;
    }
    printf (" Done\n");
    return sock;
}

int connect_device(char *addr)
{

    int sec = BT_SECURITY_LOW;
    uint8_t dst_type = BDADDR_LE_PUBLIC;
    bdaddr_t src_addr, dst_addr;
    int dev_id = -1;
    int fd;
    //if (str2ba (argv[2], &dst_addr) < 0)
    pthread_mutex_lock(&mutex);
    if (str2ba (addr, &dst_addr) < 0)
    {
        perror ("Invalid remote address:\n");
        return EXIT_FAILURE;
    }

    if (dev_id == -1)
    {
        bacpy (&src_addr, BDADDR_ANY);
    }
    else if (hci_devba (dev_id, &src_addr) < 0)
    {
        perror ("Adapter not available");
        return EXIT_FAILURE;
    }

    fd = l2cap_le_att_connect (&src_addr, &dst_addr, dst_type, sec);
    pthread_mutex_unlock(&mutex);
    return fd;
}


void Get(int fd)
{
    key_value_pair_t *pairs = NULL;
    key_value_pair_t *pairs1 = NULL; // *pairs_return;
    unsigned char temp[512];
    pairs1 = form_key_value_pair(fd,characteristic_table);
    pairs = Write_to_pair(fd,characteristic_table);
    /* if(strcasecmp(g_address, "90:59:AF:0B:7F:D4")==0)
        {*/
    // printf("address: %s", g_address);

    while (pairs != NULL)
    {
        printf("handle: %.4x",pairs->handle_res);
        printf (" %s ====> ", pairs->key);
        if(strcmp(pairs->key,"Device Name")==0)
        {
            printf("%s",pairs->value);
            strcpy(global_temp,pairs->value);    //giving device name to global_temp
        }
        else
        {
            if(pairs->value[1] == NULL)
            {
                strcpy(pairs->value, "NULL");
                printf("%s", pairs->value);
            }
            else
            {
                for (int k=0; k<20;k++)   // for (int k=1; k<20;k++)
                    printf("%.2X ",pairs->value[k]);
                int m=0;
                memset(temp, '\0',512);
                for (int n=0; n<20;n++)
                {
                    sprintf(&temp[m],"%.2X ",pairs->value[n]);
                    m=m+3;
                }
                // printf("temp : %s", temp);
                strncpy(pairs->value, temp,strlen(temp));
                // printf("\tpairs->value: %s",pairs->value);
            }
        }

        printf("\n");
        pairs = pairs->pNext;
    }
    //}
}



void Put(int fd)
{
    key_value_pair_t *pairs_new = NULL;
    key_value_pair_t *pairs1 = NULL;
    key_value_pair_t *pairs = NULL;
    //        int i,j,l,k,m,n;
    unsigned char temp[512];
    //        unsigned char temp1[512];

    /*if(strcasecmp(g_address, "90:59:AF:0B:7F:D4")==0)
        {
            pairs_new = Write_data(fd,characteristic_table);
        }
        Get(fd);*/
    //        printf("getting data\n");
    //        Get(fd);
    printf("putting data\n");
    pairs_new =  Write_data(fd,characteristic_table);
    printf("after put getting data\n");
    usleep(2);
    //        Get(fd);

    pairs = Write_to_pair(fd, characteristic_table);
    pairs1 = form_key_value_pair(fd,characteristic_table);
    while (pairs1 != NULL)
    {
        printf (" %s ====> ", pairs1->key);
        if(strcmp(pairs1->key,"Device Name")==0)
        {
            printf("%s",pairs1->value);
        }
        else
        {
            if(pairs1->value[1] == NULL)
            {
                strcpy(pairs1->value, "NULL");
                printf("%s", pairs1->value);
            }
            else
            {
                for (int i=0; i<20;i++)  //i=1
                    printf("%.2X ",pairs1->value[i]);
                int j=0;
                memset(temp, '\0',512);
                for (int l=0; l<20;l++)
                {
                    sprintf(&temp[j],"%.2X ",pairs1->value[l]);
                    j=j+3;
                }
                // printf("temp : %s", temp);
                strncpy(pairs1->value, temp,strlen(temp));
                // printf("\tpairs->value: %s",pairs->value);
            }
        }
        printf("\n");
        pairs1 = pairs1->pNext;
    }
    printf("*****\n");
}



void Observe(int fd, char *addr)
{
    // char str[10][50]= {'\0'};
    uint16_t input[10]={0};
    key_value_pair_t *pairs = NULL;
    int num;
    int i = 0;
    printf("observe\n");
    //        Get(fd);
    //        printf("after get\n");
    printf("Enter number of handles you want to observe: \t");
    scanf("%d", &num);
    for(i = 0 ;i < num ; i++)
    {
        printf("Enter handle to Observe:\t");
        scanf("%x",&input[i]);
    }

    // pairs = get_pairs(fd);
    pairs = form_key_value_pair(fd,characteristic_table);
    for(i = 0; i < num; i++)
    {
        while(pairs != NULL)
        {
            //printf("input is %.4x",input[i]);
            //if(strcasecmp(str[i],pairs->handle_res) == 0)
            if(input[i] == pairs->handle_res)
            {
                printf("comparison success\n");
                strcpy(obs_table[obs_table_index].observe_pair[i].key, pairs->key);
                //strcpy(obs_table[obs_table_index].observe_pair[i].value, pairs->value);
                memcpy(obs_table[obs_table_index].observe_pair[i].value, pairs->value,512);
                obs_table[obs_table_index].observe_pair[i].handle = pairs->handle_res;
                printf("observe --> key : %s,\t", obs_table[obs_table_index].observe_pair[i].key);
                printf(" value: \n");
                for(int w =0;w<20;w++)
                    printf("%.2x ",obs_table[obs_table_index].observe_pair[i].value[w]);
                printf("\n");
            }
            pairs =  pairs->pNext;
        }
    }
    strcpy(obs_table[obs_table_index].mac_id,addr);
    obs_table[obs_table_index].num_of_keys = num;
    printf("Subscribed to observe notifications from resource: %s\n", obs_table[obs_table_index].mac_id);
    obs_table_index++;
    return ;
}


//thread_function
void *client_observe()
{
    int fd, i, j;
    key_value_pair_t *pairs = NULL;
    while(1)
    {
        sleep(2);
        for(i = 0; i < obs_table_index; i++)
        {
            //            printf("obs_table index: %d mac_id: %s\n", i, obs_table[i].mac_id);
            //            printf("fd in thread before connect ------> %d\n", fd );
            fd = connect_device(obs_table[i].mac_id);
            //            printf("thread after connect ------> %d\n", fd );
            if(fd < 0) {
                //sleep(1);
                continue;
            }
            //pairs = get_pairs(fd);
            if (fd < 0)
                EXIT_FAILURE;
            pairs = form_key_value_pair(fd,characteristic_table);
            for(j = 0; j < obs_table[i].num_of_keys; j++)
            {

                while(pairs != NULL)
                {
                    if(strcasecmp(obs_table[i].observe_pair[j].key,pairs->key) == 0)
                    {
                        if(strcmp(obs_table[i].observe_pair[j].value,pairs->value)!=0)
                        {
                            strcpy(obs_table[i].observe_pair[j].value, pairs->value);
                            printf("MAC ID: %s \t key: %s \t", obs_table[i].mac_id, obs_table[i].observe_pair[j].key);
                            printf("updated value: ");
                            for(int v=0; v<20; v++)
                                printf("%.2x ",obs_table[i].observe_pair[j].value[v]);
                            printf("\n");
                        }
                        // else printf("same \n");
                    }
                    pairs =  pairs->pNext;
                }
            }
            //            printf("closing fd\n");
            close(fd);
            //            printf("fd after closing-------> %d\n", fd);
        }

    }
}


void Observe_cancel_address(int fd, unsigned char *addr)
{
    if(obs_table_index != 0)
    {
        for(int i=0; i< obs_table_index; i++)
        {
            if(strcmp(addr, obs_table[i].mac_id) == 0)
            {
                obs_table[i]=obs_table[i+1];
                obs_table_index--;
                obs_table[i].num_of_keys--;
                printf("stoping notifications from observer : %s", addr);
            }
        }
    }
    else
    {
        printf("No client is registered to observe\n");
    }
}

void  Observe_cancel_attribute(int fd, unsigned char *addr,unsigned char *usr_key)
{
    if(obs_table_index != 0)
    {
        for(int i=0; i< obs_table_index; i++)
        {
            if(strcmp(addr, obs_table[i].mac_id) == 0)
            {
                for(int j =0 ; j <obs_table[i].num_of_keys; j++)
                {
                    if(strcmp(usr_key,obs_table[i].observe_pair[j].key) == 0)
                    {
                        obs_table[i].observe_pair[j]=obs_table[i].observe_pair[j+1];
                        obs_table[i].num_of_keys--;
                        printf("stoping notification from observer %s attribute: %s",addr,usr_key);
                    }
                }
            }
        }
    }
    else
    {
        printf("No client is registered to observe\n");
    }
}

int main()
{
    unsigned char address[18];
    unsigned char att_key[50];
    int choice;
    int fd;  // = -1;
    //char selection = 0; // 0 -> to close fd, 1 -> fd should be available till we close the application

    /******  Thread  ******/
    pthread_t thread_id;
    int result;
    //int flag_observe = 0;
    //void *pthread_result;

    result = pthread_create(&thread_id, NULL, &client_observe, NULL);

    printf("result: %d \n", result);
    if(result!=0)
    {
        printf("Error creating pthread\n");
        exit(1);
    }
    else
        printf("Thread created\n");

    while(1)
    {

        printf("Enter address to connect:\t");
        scanf("%s", address);
        //strcpy(g_address, address);
        fd = connect_device(address);
        printf("=====================\n\tfd in main: %d\n====================\n", fd);
        if (fd < 0) {
            continue;
            //return EXIT_FAILURE;
        }
        char s[2] = { 0x00, 0x01 };
        char e[2] = { 0xff, 0xff };
        memset(characteristic_table,'\0',sizeof(characteristic_table));
        table_index = 0;
        write_to_table (fd, s, e);
        printf("******************\nEnter -> 1 to GET\n \t 2 to PUT \n \t 3 to OBSERVE\n \t 4 to CANCEL OBSERVE BY ADDRESS\n \t 5 to CANCEL OBSERVE BY KEY\n****************\n");
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:printf("GETIING\n");

            Get(fd);
            close(fd);
            break;

        case 2:printf("PUTTING\n");

            Put(fd);
            close(fd);
            break;
        case 3:printf("OBSERVING\n");
            printf("getting data before setting to observe\n");
            Get(fd);
            if(strcasecmp(MACRO_TAG_NAME,global_temp+1) == 0)
            {
                printf("put to sensor tag, to enable reading changes\n");
#ifdef MACRO_TAG_NAME
                printf("putting data to tag\n");
                Put(fd);
#endif
            }
            Observe(fd, address);
            close(fd);
            break;
        case 4:Observe_cancel_address(fd, address);
            close(fd);
            break;
        case 5:printf("Enter the attribute key to stop notifications:");
            scanf("%s",att_key);
            Observe_cancel_attribute(fd, address, att_key);
            close(fd);
            break;

        }


    }
}


