// Pylontech_emoncms A55_Functions_CAN.ino
//
// Function definitions relating to CAN

void init_can0() {
  while (CAN0.begin(MCP_ANY, CAN0_BUS_SPEED, CAN0_CLOCK_RATE) != CAN_OK) {
    Serial.println(F("Error Initializing MCP2515. Retrying in 1 sec.."));
    Serial.println();
    delay(1000);
  }
#ifdef DEVMODE
  Serial.println(F("MCP2515 Initialized."));
#endif
  CAN0.setMode(MCP_LISTENONLY);                 // Set operation mode to listen so no acks sent.
}


// read in the CAN data into our cache..
void store_can() {

  Serial.print("Rx: 0x");
  Serial.println(rxId, HEX);

  byte x;
  for (x = 0; x < len; x++) {
    can_buffer[can_buffer_ptr][x] = rxBuf[x];
  }
  can_id_buffer[can_buffer_ptr] = rxId;
  can_buffer[can_buffer_ptr][8] = len;     // for post to emoncms
  can_buffer[can_buffer_ptr][9] = len;     // for CAN replicate

  can_buffer_ptr++;
  if (can_buffer_ptr == CAN_BUFFER_SIZE) can_buffer_ptr = 0;

} // end of read_can()



// routine to process the battery data into some useful variables.
void process_can() {

  // Array to hold Request IDs found on the bus:
  // [8 bytes data] [len]
  // 0x305 - Inverter reply (we ignore)
  // 0x351 - Battery voltage + current limits
  // 0x355 - State of Health (SOH) / State of Charge (SOC)
  // 0x356 - Voltage / Current / Temp
  // 0x359 - Protection & Alarms
  // 0x35C - Battery charge request
  // 0x35E - Manufacturer name (PYLON)
  //
  // All except 0x305 to be relayed to the inverters
  // numbers are stored little-endian - ie. msb, lsb

  // loop through the buffer, newest record working backwards..

  byte x, y, buf_pos;
  buf_pos = can_buffer_ptr;
  for (x = 0; x < CAN_BUFFER_SIZE; x++) {

    if (buf_pos == 0) {
      buf_pos = CAN_BUFFER_SIZE - 1;
    } else {
      buf_pos--;
    }

    if (can_buffer[buf_pos][8] == 0) continue; // no CAN data stored

    switch (can_id_buffer[buf_pos]) {

      // 0x351:
      // Battery charge voltage, charge/discharge current limit - 6 bytes of data
      // 16 bits, unsigned int, signed int, signed int
      // V in 0.1, A in 0.1
      case  0x351:
        val_unsigned = ((int)can_buffer[buf_pos][1] << 8) + ((int)can_buffer[buf_pos][0]);

        itoa(val_unsigned, batt_charge_v, 10);
        div10(batt_charge_v);

        val_signed = ((int)can_buffer[buf_pos][3] << 8) + ((int)can_buffer[buf_pos][2]);
        if ( val_signed > 0x7FFF ) val_signed = val_signed - 0x10000;

        itoa(val_signed, batt_charge_a, 10);
        div10(batt_charge_a);

        val_signed = ((int)can_buffer[buf_pos][5] << 8) + ((int)can_buffer[buf_pos][4]);
        if ( val_signed > 0x7FFF ) val_signed = val_signed - 0x10000;
        itoa(val_signed, batt_discharge_a, 10);
        div10(batt_discharge_a);

        snprintf_P(msgString,
                   MSG_BUFFER_SIZE,
                   PSTR("ID: 0x351 ChargeV=%s, ChargeA=%s, DischargeA=%s"),
                   batt_charge_v, batt_charge_a, batt_discharge_a);

        Serial.println(msgString);

        // flag we have this info..
        data_flags  = data_flags | 1;
        can_buffer[buf_pos][8] = 0;
        break;

      // State of Charge (SOC) / State of Health (SOH) - 4 bytes of data
      // 16 bits, unsigned int
      // value as a percent
      case 0x355:

        batt_soc = ((int)can_buffer[buf_pos][1] << 8) + ((int)can_buffer[buf_pos][0]);
        batt_soh = ((int)can_buffer[buf_pos][3] << 8) + ((int)can_buffer[buf_pos][2]);

        snprintf_P(msgString,
                   MSG_BUFFER_SIZE,
                   PSTR("ID: 0x355 SOC=%d%%, SOH=%d%%"),
                   batt_soc, batt_soh);

        Serial.println(msgString);

        // flag we have this info..
        data_flags  = data_flags | 2;
        can_buffer[buf_pos][8] = 0;
        break;

      // Voltage / Current / Temp - 6 bytes of data
      // 16 bits, signed int, 2s complement
      // V in 0.01V, A in 0.1A, T in 0.1C
      case 0x356:

        val_signed = ((int)can_buffer[buf_pos][1] << 8) + ((int)can_buffer[buf_pos][0]);
        if ( val_signed > 0x7FFF ) val_signed = val_signed - 0x10000;
        itoa(val_signed, batt_volts, 10);
        div100(batt_volts);

        val_signed = ((int)can_buffer[buf_pos][3] << 8) + ((int)can_buffer[buf_pos][2]);
        if ( val_signed > 0x7FFF ) val_signed = val_signed - 0x10000;
        itoa(val_signed, batt_current, 10);
        div10(batt_current);

        val_signed = ((int)can_buffer[buf_pos][5] << 8) + ((int)can_buffer[buf_pos][4]);
        if ( val_signed > 0x7FFF ) val_signed = val_signed - 0x10000;
        itoa(val_signed, batt_temp, 10);
        div10(batt_temp);

        snprintf_P(msgString,
                   MSG_BUFFER_SIZE,
                   PSTR("ID: 0x356 Volts=%s, Current=%s, Temp=%s"),
                   batt_volts, batt_current, batt_temp);
        Serial.println(msgString);

        // flag we have this info..
        data_flags  = data_flags | 4;
        can_buffer[buf_pos][8] = 0;
        break;

      // 0x359 - Protection & Alarms

      case 0x359:

        pa_flags[0] = can_buffer[buf_pos][0];
        pa_flags[1] = can_buffer[buf_pos][1];
        pa_flags[2] = can_buffer[buf_pos][2];
        pa_flags[3] = can_buffer[buf_pos][3];

        //#ifdef DEVMODE
        Serial.print("ID: 0x359 Protection / Alarm Flags: ");
        Serial.print(binText(can_buffer[buf_pos][0]));
        Serial.print(F(" "));
        Serial.print(binText(can_buffer[buf_pos][1]));
        Serial.print(F(" "));
        Serial.print(binText(can_buffer[buf_pos][2]));
        Serial.print(F(" "));
        Serial.println(binText(can_buffer[buf_pos][3]));
        //#endif

        // flag we have this info..
        data_flags  = data_flags | 8;
        can_buffer[buf_pos][8] = 0;
        break;

      // Request flags (0x35C)
      //  byte 0: Request flags
      case 0x35c:

        req_flags = can_buffer[buf_pos][0];

        //#ifdef DEVMODE
        Serial.print(F("ID: 0x35C Request Flags: "));
        Serial.println(binText(can_buffer[buf_pos][0]));
        //#endif

        // flag we have this info..
        data_flags  = data_flags | 16;
        can_buffer[buf_pos][8] = 0;
        break;

      // Manufacturer Name: "PYLON  "
      case 0x35E:

        Serial.print(F("ID: 0x35E "));
        for (byte i = 0; i < can_buffer[buf_pos][8]; i++) {
          Serial.print(char(can_buffer[buf_pos][i]));
        }
        Serial.println();
        can_buffer[buf_pos][8] = 0;
        break;
    }
    // if we have a full set of data, end the loop..
    if (data_flags == 31) break;
  }
}

#if (defined CAN1_CS) || (defined CAN2_CS)
// replicate the CAN data onto the other busses..
void bus_replicate() {

  // Array to hold Request IDs found on the bus:
  // [8 bytes data] [len]
  // 0x305 - Inverter reply (we ignore)
  // 0x351 - Battery voltage + current limits
  // 0x355 - State of Health (SOH) / State of Charge (SOC)
  // 0x356 - Voltage / Current / Temp
  // 0x359 - Protection & Alarms
  // 0x35C - Battery charge request
  // 0x35E - Manufacturer name ("PYLON   ")
  //
  // All except 0x305 to be relayed to the inverters
  // numbers are stored little-endian - ie. msb, lsb

  // loop through the buffer, newest record working backwards..

  byte x, y, buf_pos, data_flags;
  buf_pos = can_buffer_ptr;
  for (x = 0; x < CAN_BUFFER_SIZE; x++) {

    if (buf_pos == 0) {
      buf_pos = CAN_BUFFER_SIZE - 1;
    } else {
      buf_pos--;
    }

    if (can_buffer[buf_pos][9] == 0) continue; // no CAN data stored

    switch (can_id_buffer[buf_pos]) {

      // 0x351:
      // Battery charge voltage, charge/discharge current limit - 6 bytes of data
      // possibly adjust value - if your battery pile is too small??
      case  0x351:
        send_can(buf_pos);
        can_buffer[buf_pos][9] = 0;
        data_flags  = data_flags | 1;
        break;

      // State of Charge (SOC) / State of Health (SOH) - 4 bytes of data
      // 16 bits, unsigned int
      // value as a percent
      case 0x355:
        send_can(buf_pos);
        can_buffer[buf_pos][9] = 0;
        data_flags  = data_flags | 2;
        break;

      // Voltage / Current / Temp - 6 bytes of data
      // 16 bits, signed int, 2s complement
      // V in 0.01V, A in 0.1A, T in 0.1C
      case 0x356:
        send_can(buf_pos);
        can_buffer[buf_pos][9] = 0;
        data_flags  = data_flags | 4;
        break;

      // 0x359 - Protection & Alarms
      case 0x359:
        send_can(buf_pos);
        can_buffer[buf_pos][9] = 0;
        data_flags  = data_flags | 8;
        break;

      // Request flags (0x35C)
      //  byte 0: Request flags
      case 0x35c:
        send_can(buf_pos);
        can_buffer[buf_pos][9] = 0;
        data_flags  = data_flags | 16;
        break;

      // Manufacturer Name: "PYLON  "
      case 0x35E:
        send_can(buf_pos);
        can_buffer[buf_pos][9] = 0;
        data_flags = data_flags | 32;
        break;
    }

    // if we have a full set of data, end the loop..
    if (data_flags == 63) break;
  }
  Serial.println();
  Serial.println();
}
#endif


#if (defined CAN1_CS) || (defined CAN2_CS)
void send_can(byte buf_pos) {

  snprintf_P(msgString,
             MSG_BUFFER_SIZE,
             PSTR("Tx: 0x%3X len: %d"),
             can_id_buffer[buf_pos], can_buffer[buf_pos][9]);
  Serial.println();
  Serial.print(msgString);
  byte x;
  for (x = 0; x < can_buffer[buf_pos][9]; x++) {
    Serial.print(" ");
    Serial.print(can_buffer[buf_pos][x], HEX);
  }

#ifdef CAN1_CS
  if (CAN1.sendMsgBuf(can_id_buffer[buf_pos],
                      can_buffer[buf_pos][9],
                      can_buffer[buf_pos]) == CAN_OK) {
    Serial.print(" (ok)");
  } else {
    Serial.print(" (fail)");
  }
#endif
#ifdef CAN2_CS
  if (CAN2.sendMsgBuf(can_id_buffer[buf_pos],
                      can_buffer[buf_pos][9],
                      can_buffer[buf_pos]) == CAN_OK) {
    Serial.print(" (ok)");
  } else {
    Serial.print(" (fail)");
  }
#endif
}
#endif
