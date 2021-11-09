// Pylontech_emoncms A55_Functions_Post.ino
//
// Function definitions relating to posting the data


// Post battery data to emoncms
// this routine is run from loop() flagged by a ticker periodically,
// interval is set in INTERVAL settings.
// simple, blocking routine.

bool post_to_emoncms () {

  // when we have a complete set of data, we send it..
  if (data_flags == 31) {

    // json the Protection & Alarm flag data..
    alarm_flags_buf[0] = 0;

    // loop through the four flag bytes..
    for (flag_byte = 0; flag_byte < 4; flag_byte++ ) {
      flag_mask = 128;  // set to highest bit
      flag_pos = 0;
      do {
        // check for bit..
        if (flags[flag_byte][flag_pos][0] != 0) {

          strcat(alarm_flags_buf, ",");
          strcat(alarm_flags_buf, flags[flag_byte][flag_pos]);

          if (pa_flags[flag_byte] & flag_mask ) {
            strcat(alarm_flags_buf, ":1");
          } else {
            strcat(alarm_flags_buf, ":0");
          }
        }

        flag_pos ++;
        flag_mask = flag_mask >> 1; // roll one bit down
      } while (flag_mask != 0);
    }

    // now json the request flags..
    request_flags_buf[0] = 0;

    flag_mask = 128;
    flag_pos = 0;

    do {
      // check for flag..
      if (flags[4][flag_pos][0] != 0) {
        strcat(request_flags_buf, ",");
        strcat(request_flags_buf, flags[4][flag_pos]);
        // check for bit..
        if (req_flags & flag_mask ) {
          strcat(request_flags_buf, ":1");
        } else {
          strcat(request_flags_buf, ":0");
        }
      }
      flag_pos ++;
      flag_mask = flag_mask >> 1;
    } while (flag_mask != 0);

#ifdef DEVMODE
    snprintf_P(msgString,
               MSG_BUFFER_SIZE,
               PSTR("Connecting to %s:%d"),
               emonHost, emonPort);
    Serial.println(msgString);
#endif

    WiFiClient client;
    if (!client.connect(emonHost, emonPort)) {
#ifdef DEVMODE
      Serial.println(F("Connection failed!"));
#endif
      delay(5000);
      // give up..
      return false;
    }

    if (client.connected()) {

      snprintf_P(msgString,
                 MSG_BUFFER_SIZE,
                 PSTR("GET /emoncms/input/post.json?apikey=%s&node=%d&json={V:%s,A:%s,SOC:%d,SOH:%d,T:%s%s%s}HTTP/1.1\r\nHost:%s\r\nConnection: close\r\n\r\n"),
                 apikey, node_id, batt_volts, batt_current,
                 batt_soc, batt_soh, batt_temp,
                 alarm_flags_buf, request_flags_buf,
                 emonHost);

#ifdef DEVMODE
      Serial.println(F("Sending.."));
      Serial.println(msgString);
#endif

      // This will send the request to the server
      client.print(msgString);

      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);

      data_flags = 0;

#ifdef DEVMODE
      while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println();
      Serial.println(F("closing connection"));
#endif
      return true;
    }
  } else {
    Serial.println("No complete data to send.");
    return false;
  }
}
