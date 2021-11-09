// Pylontech_emoncms A60_Notes.ino
//
// Notes on the sketch.

// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING /////
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING /////
// 
// Messing around with your battery system is not a good idea unless you
// really, really, know what you are doing. You will also most likely
// INVALIDATE ANY WARRANTIES.
//
// While part of this sketch is designed to replicate CAN data to additional
// inverter/chargers, IT IS YOUR RESPONSIBILITY to ensure the system behaves
// correctly and the batteries are properly managed within their operational
// parameters.
// 
// Don't come bleating if you:
//
//    * destroy your battery system.
//    * destroy your inverter/chargers.
//    * burn your house down.
//    * do anything worse.
//
// YOU HAVE BEEN WARNED!
//
// Also bear in mind that the sketch as it stands does not modify *any*
// data from the batteries sent to the inverter/chargers. This means that
// the current limits for charging and discharging will be sent to the
// inverter/charger as if it is the only one. If you have 3x inverter/
// chargers on 3x CAN buses, you therefore have the potential to send 3x 
// the maximum current to the batteries, and you should make sure your system 
// and especially cables are suitable for this or adjusted accordingly. 
// IF YOU DON'T UNDERSTAND THIS POINT FULLY, YOU SHOULD NOT BE USING THIS SKETCH.
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING /////
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING /////


// See https://setfirelabs.com for more info, 
// https://github.com/setfirelabs/Pylontech_emoncms for the latest version.

// If you are maintaining your own Git version of this sketch, you can add your
// own A05_Settings.ino file to keep your wifi and emoncms secret info.
// Add these lines to the new file (and uncomment):

//#define STASSID "your-wifi-network"
//#define STAPSK  "your-wifi-password"
//
//#define EMONHOST    "emoncms.org"
//#define EMONPORT    80
//#define EMONAPIKEY  "your-emon-api-key"
//#define EMONNODEID  node_number


// Below is an alternative non-blocking post routine to emoncms.
// Swap out with routine in A57_Functions.
// Also needs additional libraries which were not straightforward to install.
// needs:
// #include <AsyncHTTPRequest_Generic.h>     // https://github.com/khoih-prog/AsyncHTTPRequest_Generic
// ..uncommenting at the bottom of the fist .ino file

/*
void post_to_emoncms() {

  Serial.print("Full set:");
  // when we have a complete set of data, we send it..
  if (data_flags == 31) {
    Serial.println("Y");
    data_flags = 0;

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
      } while (flag_mask);
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
    } while (flag_mask);

    static bool requestOpenResult;

    if (emon_request.readyState() == readyStateUnsent
        || emon_request.readyState() == readyStateDone) {

      snprintf_P(msgString,
                 MSG_BUFFER_SIZE,
                 //PSTR("%s%:%d/emoncms/input/post.json?apikey=%s&node=%d&json={V:%s,A:%s,SOC:%d,SOH:%d,T:%s%s%s}"),
                 PSTR("%s:%d/emoncms/input/post.json?apikey=%s&node=%d&json={V:%s,A:%s,SOC:%d,SOH:%d,T:%s%s%s}"),
                 emonHost, emonPort,
                 apikey, node_id, batt_volts, batt_current,
                 batt_soc, batt_soh, batt_temp,
                 alarm_flags_buf, request_flags_buf);

      Serial.print(F("Sending: "));
      Serial.println(msgString);

      requestOpenResult = emon_request.open("GET", msgString);

      if (requestOpenResult) {
        // Only send() if open() returns true, or crash
        emon_request.send();

      } else {
        Serial.println(F("Failed to send request."));
      }

    } else {
      Serial.println(F("Not ready to send emon request"));
    }
  } else {
    Serial.println("N");
  }

  emon_post_flag = false;
  return true;
} // end of post_to_emoncms

 
  */
