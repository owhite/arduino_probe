// consider checking for a heart beat from the
//  master computer and if it doesnt receive one
//  it could shut all the relays off. 

#define SERIAL_SPEED  9600
#define OUTPUT_RANGE 22
#define RedPin       7
#define GrnPin       8

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool isNumeric(String str) {
  if (str.length() == 0) {
      return false;
  }
  for (int i = 0; i < str.length(); i++) {
    if (!(isDigit(str.charAt(i)))) {
      return false;
    }
  }
  return true;
}

// an interupt called when something comes over the serial. 
void serialEvent() {
  String inputString = "";
  inputString.reserve(20);

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      process_command(inputString);
      inputString = "";
    }
    else {
      inputString += inChar;
    }
  }
}

// awaits commands in the form of:
// $ command num1 num2 
// you must begin strings with '$'
// $ set_relay 1 1 :: turns relay 1 on
// $ set_relay 3 0 :: turns relay 3 off
// $ shut_off :: shuts off all relays
void process_command(String str) {
  String c = getValue(str, ' ', 0);
  String command = getValue(str, ' ', 1);
  String value1 = getValue(str, ' ', 2);
  String value2 = getValue(str, ' ', 3);
  int v1 = -1;
  int v2 = -1;

  if (isNumeric(value1)) { v1 = value1.toInt(); }
  if (isNumeric(value2)) { v2 = value2.toInt(); }
  
  // commands must begin with '$'
  if (c == '$') {

    if (command.equals("probe_device")) {
      Serial.print("relay_driver");
    }
    if (command.equals("set_relay") && v1 != -1) {
      Serial.printf("setting relay %d :: %d\n", v1, v2);
      v1--; // pin numbers start at zero, but using whole numbers for relays
      if(v2 == 1) {
	digitalWrite(v1, HIGH);
      }
     if(v2 == 0) {
	digitalWrite(v1, LOW);
      }
    }
    if (command.equals("shut_down")) {
      Serial.printf("shutdown\n");
      for(int i=0; i < OUTPUT_RANGE; i++){
	pinMode(i, OUTPUT);
	digitalWrite(i, LOW);
      }
    }
  }
}

void setup() {
  Serial.begin(SERIAL_SPEED);

  for(int i=0; i < OUTPUT_RANGE; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

}

void loop() {

  // this line should NOT be needed for most arduinos
  // but was required for a teensy2.0
  if(Serial.available()) { serialEvent(); }

}
