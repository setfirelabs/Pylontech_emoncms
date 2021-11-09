// Pylontech_emoncms A10_Settings.ino
//
// Settings etc..
// Wifi and emoncms info can be put in a separate A05_Settings.ino file.
// See A60_Notes.ino for more info.


//
// Wifi definition, if not in your own A05_Settings.ino file..
#ifndef STASSID
#define STASSID "your-wifi-ssid"
#define STAPSK  "your-wifi-password"
#endif

// Emoncms definition, if not in your own A05_Settings.ino file..
#ifndef EMONHOST
#define EMONHOST    "emoncms.org"
#define EMONPORT    80
#define EMONAPIKEY  "your-api-key"
#define EMONNODEID  1
#endif

// CAN interface settings.
// Bus speeds:  CAN_100KBPS, CAN_500KBPS, etc. Pylontech CAN bus is 500k.
// Clock rates: MCP_8MHZ, MCP_16MHZ etc.

#define CAN0_BUS_SPEED CAN_500KBPS
#define CAN0_CLOCK_RATE MCP_8MHZ
#define CAN0_INT D1
#define CAN0_CS  D2

#define CAN1_BUS_SPEED CAN_500KBPS
#define CAN1_CLOCK_RATE MCP_8MHZ
// comment out to disable interface:
#define CAN1_CS  D3

#define CAN2_BUS_SPEED CAN_500KBPS
#define CAN2_CLOCK_RATE MCP_8MHZ
// comment out to disable interface:
#define CAN2_CS D4

#define CAN_BUFFER_SIZE 32

// SPI on NodeMCU with NodeMCU as Master:
//           NodeMCU -- MCP2515_CAN
// CLK / M-CLK => D5 -- SCK
//    Q / MISO => D6 -- SO
//    D / MOSI => D7 -- SI

// set to 1 for debug messages..
#define DEV_MODE 1

// for AsyncHTTPRequest_Generic
// Level from 0-4
#define ASYNC_HTTP_DEBUG_PORT     Serial
#define _ASYNC_HTTP_LOGLEVEL_     1

// Timing intervals
#define INTERVAL_CAN 5       // seconds for CAN replicate
//#define INTERVAL_DISPLAY 10  // multiple of INTERVAL_CAN to print buffer to serial (comment out to remove)
#define INTERVAL_EMONPOST  12 // multiple of INTERVAL_CAN to post to emoncms (5x12 = 60 seconds)

// How much unrecognised rubbish from the CAN bus we tolerate before resetting the interface.
// This is used to re-initialise the CAN interface if it incorrectly syncs it's clock
// eg. connecting to the bus when already powered up.
#define MAX_CRUD_COUNT 10
