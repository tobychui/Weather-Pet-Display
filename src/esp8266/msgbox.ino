/*
 * 
 *  Msgbox
 *  
 *  This script handle the message box functions
 *  
 *  
 *  Gesture Code:
 *  
 */

 
int messageBoxCountdown = 0; //In 100ms

//Update
void updateMessageboxStatus(){
  if (messageBoxCountdown <= 0){
    //Hide messagebox
    hideMessageBox();
    //Reset facial expression
    changeFacialExpression(0);
  }else{
    messageBoxCountdown--;
  }
}

//Show messagebox, message as string and duration in ms
void msgBox(String message, int duration){
  messageBoxCountdown = duration / 100;
  showMessageBoxWithText(message);
}

/*
 *  Code related to web access endpoint
 */

//Handlers for Web Server
void handle_index() {
  server.send(200, "text/html", "ArOZ DSSP - Prototype"); 
}


//Getting new message. Display it on UI
void handle_msg() {
  String value = server.arg("value");
  msgBox(value, 3000);
  server.send(200, "text/html", "OK"); 
}

//Getting new facial expression
void handle_face() {
  String value = server.arg("value");
  int newFaceID = value.toInt();
  changeFacialExpression(newFaceID);
  server.send(200, "text/html", "OK"); 
}



void handle_status() {
  server.send(200, "application/json", "{\"Temp\":\"" + currentTemp + "\", \"Humd\":\"" + currentHumd + "\"}"); 
}

void handle_endpoints() {
  server.send(200, "application/json", "[{\
    \"Name\": \"Message\",\
    \"RelPath\":\"msg\",\
    \"Desc\":\"Show message on weather display\",\
    \"Type\":\"string\",\
    \"Regex\":\"/[A-Za-z0-9]/g\",\
    \"AllowRead\":false,\
    \"AllowWrite\":true\
  }, {\
    \"Name\": \"Face\",\
    \"RelPath\":\"face\",\
    \"Desc\":\"Change facial expression\",\
    \"Type\":\"integer\",\
    \"Min\":0,\
    \"Max\":6,\
    \"AllowRead\":false,\
    \"AllowWrite\":true\
  }]"); 
}


/*
 *  Code related to MDNS broadcast and ArozOS HDSv2 Scanner
 */

 
//Inject zeroconf attr into the MDNS respond (For scanning by ArozOS)
void MDNSDynamicServiceTxtCallback(const MDNSResponder::hMDNSService p_hService) {
    //Define the domain of the HDSv2 devices
    MDNS.addDynamicServiceTxt(p_hService, "domain","hds.arozos.com");
    MDNS.addDynamicServiceTxt(p_hService, "protocol","hdsv2");

    //Define the OEM written values
    MDNS.addDynamicServiceTxt(p_hService, "uuid",getMacAddress());
    MDNS.addDynamicServiceTxt(p_hService, "model","Display");
    MDNS.addDynamicServiceTxt(p_hService, "vendor","HomeDynamic Project");
    MDNS.addDynamicServiceTxt(p_hService, "version_minor","0.00");
    MDNS.addDynamicServiceTxt(p_hService, "version_build","0");
}
  

void hostProbeResult(String p_pcDomainName, bool p_bProbeResult) {
  MDNS.setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallback);
}
