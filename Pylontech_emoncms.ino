/*
  CAN Bus Pylontech Reader by Jonathan Clark, SetfireLabs.com
  https://github.com/setfirelabs/Pylontech_emoncms

  Sketch for listening on the CAN bus of a Pylontech US2000 / US3000 battery or stack
  and outputting data to emoncms.org or a private emoncms installation.
  At the same time, able to replicate the CAN data onto additional CAN buses.
  
  Hardware: NodeMCU/EPC8266 + MCP2515_CAN modules (with 8MHz clock - change MCP_8MHZ as needed)

  Pylontech CAN Bus is 500kHz, this sketch is based on the V1.2 Pylontech CAN protocol document.
  CAN library: https://github.com/coryjfowler/MCP_CAN_lib

  Currently set up to use a basic http method to post to emomcms which is blocking. 

  See the A60_Notes.ino for more information on this sketch.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#if !( defined(ESP8266) )
#error This code is intended to run on the ESP8266 platform! Please check your Tools->Board setting.
#endif

#include <ESP8266WiFi.h>
#include <mcp_can.h>                      // https://github.com/coryjfowler/MCP_CAN_lib
#include <SPI.h>
#include <Ticker.h>

// for alternative non-blocking web post..
//#include <AsyncHTTPRequest_Generic.h>     // https://github.com/khoih-prog/AsyncHTTPRequest_Generic
