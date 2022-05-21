/*
 * 
 *  HMI Communication Script
 *  
 *  This script helps you to communicate with the HMI display
 *  from 
 */
// Send command to the UART HMI
// Example usage: sendCommand("t3.txt=\"OUTPUT\"");

bool nextReadIsPageNumber = false; //Next byte is page number
byte incomingByte = 0;             //Incoming byte buffer

void sendCommand(String cmd){
  if (debugMode){
    //Mirror output to serial
    Serial.print(cmd);
    Serial.write(0XFF);        
    Serial.write(0XFF);
    Serial.write(0XFF);
  }
  HMISerial.print(cmd); 
  HMISerial.write(0XFF);
  HMISerial.write(0XFF);
  HMISerial.write(0XFF);
  delay(100);
}

//Send Float to the display, support -99 to +99 range, automatic turncat into 4 digit with unit
//Example usage: printFloatCommand("t6.txt=", 16.8, "V");
void printFloatCommand(String cmd, float value, String unit){
  if (debugMode){
    //Mirror output to serial
    Serial.print(cmd + "\"");
    if (value <= -10){
      Serial.print(value, 1);
    }else if (value < 0){
      Serial.print(value, 2);
    }else if (value < 10){
       Serial.print(value, 2);
    }else{
      Serial.print(value, 1);
    }
    Serial.print(unit + "\"");
    Serial.write(0XFF);        
    Serial.write(0XFF);
    Serial.write(0XFF);
  }
  HMISerial.print(cmd + "\"");
  if (value <= -10){
    HMISerial.print(value, 1);
  }else if (value < 0){
    HMISerial.print(value, 2);
  }else if (value < 10){
    HMISerial.print(value, 2);
  }else{
    HMISerial.print(value, 1);
  }
  HMISerial.print(unit + "\"");
  HMISerial.write(0XFF);
  HMISerial.write(0XFF);
  HMISerial.write(0XFF);
  delay(100);
}

//Facial Expression
void changeFacialExpression(int expr){
   if (expr==0){
    //default face
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=26");
    sendCommand("expr.aph=127");
    sendCommand("expr2.aph=0");
   }else if (expr == 1){
    //happy face
    sendCommand("gm0.vid=0");
    sendCommand("expr.pic=17");
    sendCommand("expr.aph=127");
    sendCommand("expr2.aph=0");
   }else if (expr == 2){
    //shy face
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=20");
    sendCommand("expr.aph=127");
    sendCommand("expr2.pic=26");
    sendCommand("expr2.aph=127");
   }else if (expr == 3){
    //angry face
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=13");
    sendCommand("expr.aph=127");
    sendCommand("expr2.pic=22");
    sendCommand("expr2.aph=127");
   }else if (expr == 4){
    //shock face
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=19");
    sendCommand("expr.aph=127");
    sendCommand("expr2.pic=23");
    sendCommand("expr2.aph=127");
   }else if (expr == 5){
    //look away face
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=12");
    sendCommand("expr.aph=127");
    sendCommand("expr2.pic=26");
    sendCommand("expr2.aph=127");
   }else if (expr == 6){
    //look down face
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=15");
    sendCommand("expr.aph=127");
    sendCommand("expr2.aph=0");
   }else{
    //default
    sendCommand("gm0.vid=1");
    sendCommand("expr.pic=26");
    sendCommand("expr.aph=127");
    sendCommand("expr2.aph=0");
   }
}

//Show the messagebox
void showMessageBoxWithText(String stringToSend){
  sendCommand("t1.txt=\"" + stringToSend + "\"");
  sendCommand("p4.aph=100");
  sendCommand("t1.aph=127");
  sendCommand("b8.aph=127");
}

void hideMessageBox(){
  sendCommand("p4.aph=0");
  sendCommand("t1.aph=0");
  sendCommand("b8.aph=0");
}

//Get the current page number on HMI
int getPageNumber(){
  //Request the UART HMI to return its current page number
  sendCommand("sendme");

  //Try to parse and catch the return value
  bool nextReadIsPageNumber = false;
  if (debugMode){
    //Debug mode
    while (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = Serial.read();
      if (nextReadIsPageNumber){
        //This read is the page number
        nextReadIsPageNumber = false;
        return incomingByte;
      }

      if (incomingByte == 0x66){
        //This is the start of a page response
        nextReadIsPageNumber = true;
      }
    }
  }else{
    while (HMISerial.available() > 0) {
      // read the incoming byte:
      incomingByte = HMISerial.read();
      if (nextReadIsPageNumber){
        //This read is the page number
        nextReadIsPageNumber = false;
        return incomingByte;
      }

      if (incomingByte == 0x66){
        //This is the start of a page response
        nextReadIsPageNumber = true;
      }
    }
  }

  return -1;
}

void SendDateTimeChinese(String year, String month, String day){
  if (debugMode){
    Serial.print("date.txt=\"");
    Serial.print(year);
    Serial.write(0xA6); //年
    Serial.write(0x7E);
    
    Serial.print(month);
    Serial.write(0xA4); //月
    Serial.write(0xEB);
    
    Serial.print(day);
    Serial.write(0xA4); //日
    Serial.write(0xE9);
  
    Serial.print("\"");
    Serial.write(0XFF);
    Serial.write(0XFF);
    Serial.write(0XFF);
  }
  HMISerial.print("date.txt=\"");
  HMISerial.print(year);
  HMISerial.write(0xA6); //年
  HMISerial.write(0x7E);
  
  HMISerial.print(month);
  HMISerial.write(0xA4); //月
  HMISerial.write(0xEB);
  
  HMISerial.print(day);
  HMISerial.write(0xA4); //日
  HMISerial.write(0xE9);

  HMISerial.print("\"");
  HMISerial.write(0XFF);
  HMISerial.write(0XFF);
  HMISerial.write(0XFF);
  delay(100);
}

//Send Day of Week in Chinese (Big5)
void SendWeekDayChinese(int WeekDay){
  if (debugMode){
    Serial.print("dow.txt=\"");
    Serial.write(0xAC); //星
    Serial.write(0x50);
    Serial.write(0xB4);// 期
    Serial.write(0xC1);
    switch(WeekDay){
      case 0:
        Serial.write(0xA4); //日
        Serial.write(0xE9);
        break;
      case 1:
        Serial.write(0xA4); //一
        Serial.write(0x40);
        break;
      case 2:
        Serial.write(0xA4); //二
        Serial.write(0x47);
        break;
      case 3:
        Serial.write(0xA4); //三
        Serial.write(0x54);
        break;
      case 4:
        Serial.write(0xA5); //四
        Serial.write(0x7C);
        break;
      case 5:
        Serial.write(0xA4); //五
        Serial.write(0xAD);
        break;
      case 6:
        Serial.write(0xA4); //六
        Serial.write(0xBB);
        break;
    }
    Serial.print("\"");
    Serial.write(0XFF);
    Serial.write(0XFF);
    Serial.write(0XFF);
    delay(100);
  }else{
    HMISerial.print("dow.txt=\"");
    HMISerial.write(0xAC); //星
    HMISerial.write(0x50);
    HMISerial.write(0xB4);// 期
    HMISerial.write(0xC1);
    switch(WeekDay){
      case 0:
        HMISerial.write(0xA4); //日
        HMISerial.write(0xE9);
        break;
      case 1:
        HMISerial.write(0xA4); //一
        HMISerial.write(0x40);
        break;
      case 2:
        HMISerial.write(0xA4); //二
        HMISerial.write(0x47);
        break;
      case 3:
        HMISerial.write(0xA4); //三
        HMISerial.write(0x54);
        break;
      case 4:
        HMISerial.write(0xA5); //四
        HMISerial.write(0x7C);
        break;
      case 5:
        HMISerial.write(0xA4); //五
        HMISerial.write(0xAD);
        break;
      case 6:
        HMISerial.write(0xA4); //六
        HMISerial.write(0xBB);
        break;
    }
    HMISerial.print("\"");
    HMISerial.write(0XFF);
    HMISerial.write(0XFF);
    HMISerial.write(0XFF);
    delay(100);
  }
 
}
