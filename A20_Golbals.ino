// Pylontech_emoncms A20_Globals.ino
//
// Global variables etc..


const char* ssid     = STASSID;
const char* password = STAPSK;

const char* emonHost = EMONHOST;
const unsigned int emonPort = EMONPORT;
const char* apikey = EMONAPIKEY;
const byte node_id = EMONNODEID;

// Used to receive CAN data
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

#define MSG_BUFFER_SIZE  (300)              // This buffer is used lots 
char msgString[MSG_BUFFER_SIZE];            // - longest is the string for posting to emoncms

#define ALARM_BUFFER_SIZE (85)               // Buffer for the battery error flags
char alarm_flags_buf[ALARM_BUFFER_SIZE];     // max 14 flags, each 6 chars =14*6 +1 = 85.

#define REQUEST_BUFFER_SIZE (32)               // Buffer for the battery error flags
char request_flags_buf[REQUEST_BUFFER_SIZE];   // max 5 flags, each 6 chars =5*6 +1 = 31.
unsigned long time1, time2;

unsigned int val_unsigned;
signed int val_signed;
char batt_charge_v[8]; // -32768 to 32767 = max 6 chars +1 decimal point +1 null end
char batt_charge_a[8];
char batt_discharge_a[8];
char batt_volts[8];
char batt_current[8];
char batt_temp[8];
unsigned int batt_soh;
unsigned int batt_soc;
char batt_flags[14][4]; // 14 possible flags (V1.2)
byte charpos;
byte data_flags = 0;    /* bits used as flags for received info IDs:
                           bit 1 = 1 - 0x351 - ChargeV, ChargeA, DischargeA
                           bit 2 = 2 - 0x355 - SOC, SOH
                           bit 3 = 4 - 0x356 - Volts, Current, Temp
                           bit 4 = 8 - 0x359 - Alarm, Protection flags
                           bit 5 = 16 - 0x35C - Request flags
                           bit 6 = 32 - "PYLON   "
                           1+2+4+8+16 = 31 - so 31=all info collected */

// Where we hold the battery CAN data
// buffer length in settings

byte can_buffer[CAN_BUFFER_SIZE][10];
long unsigned int can_id_buffer[CAN_BUFFER_SIZE];
byte can_buffer_ptr;  // pointer to next free buffer location
uint8_t can_irq; // irq set by interrupt

byte crud_count;

byte req_flags, pa_flags[4],
     flag_mask, flag_pos, flag_byte;

// Protection / Alarm Flags
// - single bits used to flag status info
// Currently 14 flags:
// Protocol version 1.2 2018/04/08
//
// byte 0 : Protection
//  bit 7: Discharge overcurrent    PDO
//  bit 4: Cell undertemp           PUT
//  bit 3: Cell overtemp            POT
//  bit 2: Cell/module undervolt    PUV
//  bit 1: Cell/module overvolt     POV
// byte 1 : Protection
//  bit 3: System error             PSE
//  bit 0: Charge overcurrent       PCO
// byte 2 : Alarm
//  bit 7: Discharge highcurrent    ADH
//  bit 4: Cell lowtemp             ALT
//  bit 3: Cell hightemp            AHT
//  bit 2: Cell/module lowvolt      ALV
//  bit 1: Cell/module highvolt     AHV
// btye 3 : Alarm
//  bit 3: Internal comms fail      ACF
//  bit 0: Charge highcurrent       ACH
// byte 4: Module numbers - 8 bits unsigned - what is this? (ignoring)

// Request flags (0x35C)
//  byte 0: Request flags
//   bit 7: Charge enable           RCE
//   bit 6: Discharge enable        RDE
//   bit 5: Request force charge 1  RF1
//   bit 4: Request force charge 2  RF2
//   bit 3: Request full charge     RFC

const char flags[5][8][4] = {
  // Protection / Alarm flags (ID 0x359)
  // byte 0, bits 7 down to 0..
  { "PDO", "", "", "PUT", "POT", "PUV", "POV", "" },
  // byte 1
  { "", "", "", "", "PSE", "", "", "PCO" },
  // byte 2
  { "ADH", "", "", "ALT", "AHT", "ALV", "AHV", "" },
  // byte 3
  { "", "", "", "", "ACF", "", "", "ACH" },
  // Request flags (ID 0x35C)
  { "RCE", "RDE", "RF1", "RF2", "RFC", "", "", "" }
};

// Set up CAN interfaces..
MCP_CAN CAN0(CAN0_CS); 
#ifdef CAN1_CS
MCP_CAN CAN1(CAN1_CS); 
#endif
#ifdef CAN2_CS
MCP_CAN CAN2(CAN2_CS); 
#endif


bool can_flag, emon_post_flag, display_flag;
byte counter_emon_post, counter_display;
Ticker timing;


WiFiClient  client;
//AsyncHTTPRequest emon_request;
