float getSupplyVolts() {
  ads.setVoltageReference(REF_EXTERNAL);
  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  delay(50);
  float volts = 3.59 + ads.readDifferential_2_3()  * 3.3 / pow(2, 23);

  //if ( abs(volts - pre_volts) >= 0.2);  pre_volts = volts;
  //volts = 0.99 * pre_volts + 0.01 * volts;
  //pre_volts = volts;
 

  if (abs(pre_volts - volts) <= 0.02) {
    volts = 0.99 * pre_volts + 0.01 * volts;
  }
  pre_volts = volts;
   volts = round(volts * 100) / 100.0;
  return volts;
}


int getMV() {
  ads.setVoltageReference(REF_INTERNAL);
  return ads.readDifferential_0_1() * 2.048 / pow(2, 23) * 1000.0;
}


float getABS() {
  MBregisters[MV_REG] = getMV();
  float abs_val;
  if (MBregisters[MV100_REG] != MBregisters[MV0_REG])
    abs_val = (float)(MBregisters[MV_REG] - MBregisters[MV0_REG]) / (MBregisters[MV100_REG] - MBregisters[MV0_REG]) * 100.0 ;
  else
    abs_val = 0;

  if (abs_val > 110) {
    abs_val = -1;
  } else if (abs_val > 100) {
    abs_val = 100;
  } else if (abs_val < 0) {
    abs_val = 0;
  }

  if (MBregisters[STATUS_REG] == 1) {
    // Test running: accumulate running average
    if (MBregisters[COUNTER_REG] != testLen)
      _SDR = (_SDR * (testLen - MBregisters[COUNTER_REG] - 1) + abs_val) / (float)(testLen - MBregisters[COUNTER_REG]);
    else
      _SDR = 0;
  }
  else if (MBregisters[STATUS_REG] == 0) {
    // Idle / reset
    _SDR = 0;
  }
  // STATUS == 2 (complete): hold the final SDR value, do not keep averaging
  return abs_val;
}


float getRT() {
  ads.setVoltageReference(REF_INTERNAL);
  digitalWrite(PIN_A, HIGH);
  digitalWrite(PIN_B, LOW);
  delay(10);
  float detV = ads.readDifferential_2_3()  * 2.048 / pow(2, 23);
  float res = 0.4 * 100 / (0.2 - 2 * detV) - 100;
  if (abs(res - pre_res) > 5) pre_res = res;
  res = 0.7 * pre_res + 0.3 * res;
  pre_res = res;
  return res;
}

float resToTemp(float r) {
  if (r > 100) {
    float a = 100 * B;
    float b = 100 * A;
    float c = 100 - r;
    float d = b * b - 4 * a * c;
    return (-b + sqrt(d)) / (2 * a);
  } else if ( r < 100) {
    float t0 = -4;
    float t1, er;
    do {
      t1 = t0 - ft(t0, r) / f_t(t0);
      er = abs(t1 - t0);
      t0 = t1;
    } while (er > 0.001);

    return t0;
  } else {

    return 0;
  }
}



float ft(float t, float r1) {
  return 100 * (1 + A * t + B * pow(t, 2) + C * pow(t, 3) * (t - 100)) - r1;
}

float f_t(float t) {
  return 100 * (A + 2 * B * t + 3 * C * pow(t, 2) * (t - 100) + C * pow(t, 3));
}
