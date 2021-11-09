// Pylontech_emoncms A30_Setup.ino
//
// Setup routine

void setup() {
  Serial.begin(115200);
  Serial.println(F("Booting."));

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);

#ifdef DEVMODE
  Serial.print(F("Starting WiFi with:"));
  Serial.print(ssid);
  Serial.print(F(":"));
  Serial.println(password);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {

    Serial.println(F("Connection Failed! Retrying..."));
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  //#ifdef DEVMODE
  Serial.println(F("Ready"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  //#endif

  // initialise the CAN interfaces..
  init_can0();
  pinMode(CAN0_INT, INPUT);                     // Configuring pin for /INT input

#ifdef CAN1_CS
  while (CAN1.begin(MCP_ANY, CAN1_BUS_SPEED, CAN1_CLOCK_RATE) != CAN_OK) {
    Serial.println(F("Error Initializing CAN1 MCP2515. Retrying in 1 sec.."));
    Serial.println();
    delay(1000);
  }
#ifdef DEVMODE
  Serial.println(F("CAN1 MCP2515 Initialized."));
#endif
  CAN1.setMode(MCP_NORMAL);
#endif
#ifdef CAN2_CSt
  while (CAN2.begin(MCP_ANY, CAN2_BUS_SPEED, CAN2_CLOCK_RATE) != CAN_OK) {
    Serial.println(F("Error Initializing CAN2 MCP2515. Retrying in 1 sec.."));
    Serial.println();
    delay(1000);
  }
#ifdef DEVMODE
  Serial.println(F("CAN2 MCP2515 Initialized."));
#endif
  CAN2.setMode(MCP_NORMAL);
#endif

  // flush the CAN data store..
  for (can_buffer_ptr = 0; can_buffer_ptr < CAN_BUFFER_SIZE; can_buffer_ptr++) {
    // up to 8 bytes of CAN data,
    // 9th is length for emon,
    // 10th is the length for CAN replication.
    can_buffer[can_buffer_ptr][8] = 0;
    can_buffer[can_buffer_ptr][9] = 0;
  }
  can_buffer_ptr = 0; // start storing at zero

  // start ticker..
  timing.attach(INTERVAL_CAN, update_timing_flags);

#ifdef DEVMODE
  Serial.println(F("Setup finished."));
#endif

} // end of setup()
