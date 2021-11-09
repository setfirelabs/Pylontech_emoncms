// Pylontech_emoncms A50_Functions.ino
//
// Misc function definitions


// divide a string representation of a number by 10
// expecting a char array no bigger than 8!
void div10(char* returnstring) {

  if (returnstring[1] == 0) return;

  // work forwards until we find the terminator..
  charpos = 1;
  do {
    ++charpos;
    if (returnstring[charpos] == 0) {
      // we found the terminator..
      returnstring[charpos + 1] = 0;
      returnstring[charpos] = returnstring[charpos - 1];
      returnstring[charpos - 1] = 46; // decimal point
      break;
    }
  } while (charpos < 6);
}

// divide a string representation of a number by 100
// expecting a char array no bigger than 8!
void div100(char* returnstring) {
  if (returnstring[1] == 0 || returnstring[2] == 0) return;

  // work forwards until we find the terminator..
  charpos = 2;
  do {
    ++charpos;
    if (returnstring[charpos] == 0) {
      // we found the terminator..
      returnstring[charpos + 1] = 0;
      returnstring[charpos] = returnstring[charpos - 1];
      returnstring[charpos - 1] = returnstring[charpos - 2];
      returnstring[charpos - 2] = 46; // decimal point
      break;
    }
  } while (charpos < 5);
}

// display a binary number as a text string (for serial output)
char *binText(byte n) {
  static char bin[9];
  byte x;

  for (x = 0; x < 8; x++) {
    bin[x] = n & 128 ? '1' : '0';
    n <<= 1;
  }
  bin[8] = 0;

  return bin;
}

// ticker routine runs automatically as an interrupt
void update_timing_flags () {
  can_flag = true;
  counter_emon_post++;
  if (counter_emon_post == INTERVAL_EMONPOST) {
    emon_post_flag = true;
    counter_emon_post = 0;
  }
#ifdef INTERVAL_DISPLAY
  counter_display++;
  if (counter_display == INTERVAL_DISPLAY) {
    display_flag = true;
    counter_display = 0;
  }
#endif
}

// output the current stored CAN data to serial..
void display_buffer() {
  byte x, y, buf_pos;
  buf_pos = can_buffer_ptr;
  for (x = 0; x < CAN_BUFFER_SIZE; x++) {

    if (buf_pos == 0) {
      buf_pos = CAN_BUFFER_SIZE - 1;
    } else {
      buf_pos--;
    }
    snprintf_P(msgString,
               MSG_BUFFER_SIZE,
               PSTR("%2d %4X "),
               buf_pos, can_id_buffer[buf_pos]);
    Serial.print(msgString);

    for (y = 0; y < 10; y++) {

      snprintf_P(msgString,
                 MSG_BUFFER_SIZE,
                 PSTR("%.2lX "),
                 can_buffer[x][y]);
      Serial.print(msgString);
    }
    Serial.println();
  }
  Serial.println();

  //  Serial.print(F("Free RAM: "));
  //  Serial.println(ESP.getFreeHeap());
}
