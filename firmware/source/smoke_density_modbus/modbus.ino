//ModbusMessage FC01(ModbusMessage request) {
//  uint16_t addr = 0;        // Start address to read
//  uint16_t wrds = 0;        // Number of words to read
//  ModbusMessage response;
//  request.get(2, addr);
//  request.get(4, wrds);
//
//  printRequest();
//
//  if (addr < 0 || addr > nCoils_byte * 8 - 1) {
//    // No. Return error response
//    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
//    return response;
//  }
//
//  // Number of words valid?
//  if (!wrds || (addr + wrds) > (nCoils_byte * 8 )) {
//    // No. Return error response
//    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
//    return response;
//  }
//
//  //Serial.print("Addr:"); Serial.println(addr);
//  //Serial.print("datr:"); Serial.println(wrds);
//
//  uint8_t no_reg;
//  if (wrds % 8 == 0) no_reg = wrds / 8;
//  else no_reg = (wrds / 8 + 1);
//  uint8_t word_array[no_reg + 1];
//  word_array[0] = no_reg;
//  int count = 0;
//  for (int i = 1; i < no_reg + 1; i++) {
//    for (int j = 0; j < 8; j++) {
//
//      bitWrite(word_array[i], j, MBcoils[addr + count]);
//      //Serial.println(MBcoils[addr + count]);
//
//      count++;
//      if (addr + count > 15) {
//        break;
//      }
//    }
//  }
//
//
//
//  //Serial.println(word_array[1]);
//
//  response.setMessage(request.getServerID(), request.getFunctionCode(), no_reg + 1, word_array);
//
//  //  for (auto& byte : response) {
//  //    Serial.printf("%02X ", byte);
//  //  }
//  //  Serial.println();
//
//
//  return response;
//}


//ModbusMessage FC05(ModbusMessage request) {
//  uint16_t addr = 0;        // Start address to read
//  uint16_t datr = 0;        // Data of a write_register()
//  ModbusMessage response;
//
//  request.get(2, addr);
//  request.get(4, datr);
//
//  printRequest();
//
//  if (addr < 0 || addr > nCoils_byte * 8 - 1) {
//    // No. Return error response
//    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
//    return response;
//  }
//  if (datr) {
//    MBcoils[addr] = true;
//  } else {
//    MBcoils[addr] = false;
//  }
//
//  digitalWrite(RelayPins[addr], !MBcoils[addr]);
//
//
//  //Serial.print("Addr:"); Serial.println(addr);
//  //Serial.print("datr:"); Serial.println(datr);
//
//  //response.setMessage(request.getServerID(), request.getFunctionCode(), no_reg + 1, word_array);
//
//  return request;
//
//}


ModbusMessage FC06(ModbusMessage request) {
  /*
    ModbusMessage(uint8_t serverID, uint8_t functionCode, uint16_t p1, uint16_t p2)
    This constructor will be one of the most used throughout, as the most relevant Modbus
    messages have this signature (taking two 16bit parameters). These are the FCs 0x01, 0x02, 0x03, 0x04, 0x05 and 0x06.
  */
  // WRITE_HOLD_REGISTER     = 0x06,
  // Worker function for serverID=1
  //Serial.print("FC06: Function Code: ");
  //Serial.println(request.getFunctionCode());
  uint16_t addr = 0;        // Start address to read
  uint16_t datr = 0;        // Data of a write_register()
  ModbusMessage response;
  // Prints the request message
  printRequest();

  // Get addr and words from data array. Values are MSB-first, get() will convert to binary

  request.get(2, addr);
  request.get(4, datr);
  //  Serial.print("Addr:"); Serial.println(addr);
  //  Serial.print("datr:"); Serial.println(datr);

  // address valid?
  // Valid addresses are 0 .. nInputRegisters-1
  if (addr >= nInputRegisters) {
    // No. Return error response
    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    return response;
  }

  MBregisters[addr] = datr;

  setValues(addr, 0);


  // Prepare response
  //response.setMessage(request.getServerID(), request.getFunctionCode());
  //  response.add(request.getServerID(), request.getFunctionCode(), SUCCESS);
  //
  //  for (auto& byte : response) {
  //    Serial.printf("%02X ", byte);
  //  }
  //  Serial.println();

  // Return the data response
  return request;
}


ModbusMessage FC10(ModbusMessage request) {
  uint16_t addr = 0;        // Start address to read
  uint16_t len = 0;        // Data of a write_register()

  //  Serial.print("FC06: Function Code: ");
  //  Serial.println(request.getFunctionCode());

  ModbusMessage response;
  // Prints the request message


  printRequest();

  request.get(2, addr);
  request.get(4, len);

  // Whole block must fit inside the register table (0 .. nInputRegisters-1)
  if (!len || (uint32_t)addr + len > nInputRegisters) {
    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    return response;
  }

  // Request must actually carry len data words (7 header bytes + 2 per word)
  if (request.size() < 7 + 2 * (uint32_t)len) {
    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_VALUE);
    return response;
  }

  for (int i = 0; i < len; i++) {
    request.get(7 + 2 * i, MBregisters[addr + i]);

    setValues(addr, i);



    //Serial.println(MBregisters[addr + i],HEX);
  }
  //   Serial.println(MBregisters[6],HEX);
  //   Serial.println(MBregisters[7],HEX);
  //
  //  Serial.print("Addr:"); Serial.println(addr);
  //  Serial.print("len:"); Serial.println(len);

  uint8_t word_array[4] = {(uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), (uint8_t)(len >> 8), (uint8_t)(len & 0xFF)};

  response.setMessage(request.getServerID(), request.getFunctionCode(), 4, word_array);

  //  for (auto& byte : response) {
  //    Serial.printf("%02X ", byte);
  //  }
  //  Serial.println();

  return response;
}


ModbusMessage FC03(ModbusMessage request) {
  // READ_HOLD_REGISTER      = 0x03,
  // READ_INPUT_REGISTER     = 0x04,
  // Worker function for serverID=1
  // holding_memo[16]
  //  Serial.print("FC06: Function Code: ");
  //  Serial.println(request.getFunctionCode());
  uint16_t addr = 0;        // Start address to read
  uint16_t wrds = 0;        // Number of words to read
  ModbusMessage response;

  //printRequest();

//  for (auto& byte : request) {
//    Serial.printf("%02X ", byte);
//  }
//  Serial.println();

  // Get addr and words from data array. Values are MSB-first, get() will convert to binary
  request.get(2, addr);
  request.get(4, wrds);


  //  Serial.print("Addr:"); Serial.println(addr);
  //  Serial.print("datr:"); Serial.println(wrds);

  // address valid?
  if (addr >= nInputRegisters) {
    // No. Return error response
    //Serial.println("address not valid");
    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    return response;
  }

  // Number of words valid? Last register read is addr+wrds-1, must be <= nInputRegisters-1
  if (!wrds || (uint32_t)addr + wrds > nInputRegisters) {
    // No. Return error response
    //Serial.println("words not valid");
    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
    return response;
  }

  // Prepare response
  response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(wrds * 2));

  // Loop over all words to be sent
  for (uint16_t i = 0; i < wrds; i++) {
    // Add word MSB-first to response buffer
    response.add(MBregisters[addr + i]);
  }

  
//    for (auto& byte : response) {
//      Serial.printf("%02X ", byte);
//    }
//    Serial.println();

  // Return the data response
  return response;
}


void setValues(uint16_t ad, uint16_t offset) {
  if (ad + offset == DC_VOLTS_WRITE_REG) {
    dacWrite(DAC1, MBregisters[DC_VOLTS_WRITE_REG]);
    pre_volts = 0;
    writeEEPROM = true;
  } else if (ad + offset == STATUS_REG) {
    if (MBregisters[STATUS_REG] == 0) {
      testLen = requestedTestLen();
      MBregisters[COUNTER_REG] = testLen;
    }
  } else if (ad + offset == DURATION_REG) {
    // New length takes effect immediately only while idle; otherwise at the next reset.
    if (MBregisters[STATUS_REG] == 0) {
      testLen = requestedTestLen();
      MBregisters[COUNTER_REG] = testLen;
    }
  } else if (ad + offset == FUNCTION_REG) {
    if (MBregisters[FUNCTION_REG] == 1) {
      MBregisters[MV0_REG] = MBregisters[MV_REG];
      writeEEPROM = true;
    }
    else if (MBregisters[FUNCTION_REG] == 2) {
      MBregisters[MV100_REG] = MBregisters[MV_REG];
      writeEEPROM = true;
    }
    MBregisters[FUNCTION_REG] = 0;
  } else if (ad + offset == BIT_REG) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(RelayPins[i], !bitRead(MBregisters[BIT_REG], i));
    }
  }
}


void printRequest() {
  //    for (auto& byte : request) {
  //      Serial.printf("%02X ", byte);
  //    }
  //    Serial.println();
}
