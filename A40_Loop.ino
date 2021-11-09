// Pylontech_emoncms A40_Loop.ino
//
// Main loop

void loop() {
  // If CAN0_INT pin is low, read receive buffer
  if (!digitalRead(CAN0_INT)) {

    while ( CAN0.readMsgBuf(&rxId, &len, rxBuf) != CAN_NOMSG ) {
      store_can();
    }
    // maybe check for errors here?
    // then crud_count++;
  }

  if (crud_count == MAX_CRUD_COUNT) {
    // re-initialise the interface to perform a reset..
    crud_count = 0;

    Serial.println(F("Detected CAN crud - resetting interface CAN0.."));
    init_can0();
  }

  // ticker timer sets this flag for posting..
  if (emon_post_flag) {
    process_can();
    if (post_to_emoncms()) {
      emon_post_flag = false;
    } else {
      // nothing to post? Something is wrong..
      ESP.restart();
      delay(1000);
    }
  }

  if (display_flag) {
    display_buffer();
    display_flag = false;
  }

#if (defined CAN1_CS) || (defined CAN2_CS)
  if (can_flag) {

    // replicate can messages via other interfaces..
    bus_replicate();
    can_flag = false;
  }
#endif

} // end of loop()
