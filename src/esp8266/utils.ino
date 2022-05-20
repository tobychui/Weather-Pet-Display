/*
 *  Utiles
 *  
 *  As its name suggest, it is a utility function file
 */

 String timeZeroPad(int input){
  if (input < 10){
    return "0" + String(input);
  }
  return String(input);
 }

String MACString;
const char* getMacAddress(){
  unsigned char mac[6];
  WiFi.macAddress(mac);
  MACString = "";
  for (int i = 0; i < 6; ++i) {
    MACString += String(mac[i], 16);
    if (i < 5){
      MACString += '-';
    }
  }
  
  const char* _result = MACString.c_str();
  return _result;
}

 //Set the mascot to certain clothes set
 void SetClothesSet(int cs){
   if (cs == 0){
      sendCommand("cl1.pic=9");
      sendCommand("cl1.aph=127");
      sendCommand("cl2.pic=4");
      sendCommand("cl2.aph=127");
   }else if (cs == 1){
      sendCommand("cl1.pic=10");
      sendCommand("cl1.aph=127");
      sendCommand("cl2.pic=8");
      sendCommand("cl2.aph=127");
   }else if (cs == 2){
      sendCommand("cl1.pic=10");
      sendCommand("cl1.aph=127");
      sendCommand("cl2.pic=8");
      sendCommand("cl2.aph=127");
   }else if (cs == 3){
      sendCommand("cl1.aph=0");
     sendCommand("cl2.pic=8");
      sendCommand("cl2.aph=127");
   }else if (cs == 4){
      sendCommand("cl1.pic=11");
      sendCommand("cl1.aph=127");
      sendCommand("cl2.pic=7");
      sendCommand("cl2.aph=127");
   }else if (cs == 5){
      sendCommand("cl1.pic=3");
      sendCommand("cl1.aph=127");
      sendCommand("cl2.aph=0");
   }else{
     //Default
     sendCommand("cl1.aph=0");
     sendCommand("cl2.aph=0");
   }
 }
