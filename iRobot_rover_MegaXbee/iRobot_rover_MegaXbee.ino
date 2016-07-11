  // Remote codes
  #define UP 0x66
  #define DOWN 0x26
  #define LEFT 0x56
  #define RIGHT 0x16
  
  // Test LEDs
  #define rdLED 50
  #define blLED 51
  #define grLED 30
  #define ylLED 53
  
  // iRobot COI commands
  #define Start 128
  #define SafeMode 131
  #define SensorRqst 142  //[Packet ID]
  #define Bumper 7
  #define LFrontCliff 29
  #define RFrontCliff 30
  #define LCliff 28
  #define RCliff 31
  #define Drive 137       //[Vel. Hi] [Vel Low] [Rad. Hi] [Rad. Low]
  #define SPEEDF 0x0064
  #define SPEEDT 0x0032
  
  // Direction encoding
  #define NORTH 0
  #define SOUTH 1
  #define EAST 2
  #define WEST 3

  unsigned int Thresholds[4];
  boolean Cliffs[4] = {true,true,true,true};
  byte cur_dir;
  long distancia;
  long tiempo;
  int i;
  byte msg;
  byte ab[] = {0,0,0,0,0,0,0,0,0,0};  
  
  void setup(){
    pinMode(9, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
    pinMode(8, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/
    //Set up test LEDs
    pinMode(rdLED, OUTPUT);
    pinMode(grLED, OUTPUT);
    pinMode(blLED, OUTPUT);
    pinMode(ylLED, OUTPUT);
    pinMode(13,OUTPUT);
    
    digitalWrite(rdLED, LOW);
    digitalWrite(grLED, LOW);
    digitalWrite(ylLED, LOW);
    digitalWrite(blLED, LOW);
    delay(3000); // Needed to let the robot initialize
    Serial3.begin(57600);
    Serial3.write((byte)Start); // This command starts the COI.
    Serial3.write((byte)SafeMode); // Seet mode to safe 
    //Xbee-Connection
    Serial2.begin(9600);
    Serial.begin(9600);
    CalibrateCliffs();
    while (Cliffs[0]&&Cliffs[3]){
      Move(0xff9c,0x8000); //Back
      CheckCliffs();
    }
    center();
    Move(0x0000,0x0000);//Stop
    cur_dir = NORTH;
  }
  
  void loop(){
    int c=0;    
    do{
      digitalWrite(13,HIGH);
      while (Serial2.available() == 0) {
      }
      digitalWrite(13,LOW);
      ab[c]=Serial2.read();
      Serial.print(ab[c],HEX);
      Serial.print("  ");
      Serial.println(c);
      c++;
    }while (c < 10);
    msg= ab[8];
      Serial.print("msg= ");
      Serial.println(msg,HEX);
/*    switch(msg){
      case UP:
        digitalWrite(rdLED, HIGH);
        break;
      case DOWN:
        digitalWrite(grLED, HIGH);
        break;
      case LEFT:
        digitalWrite(ylLED, HIGH);
        break;
      case RIGHT:
        digitalWrite(blLED, HIGH); 
        break;
      default:
        digitalWrite(rdLED, HIGH);
        digitalWrite(grLED, HIGH);
        digitalWrite(ylLED, HIGH);
        digitalWrite(blLED, HIGH);
    }*/
    digitalWrite(13,HIGH);
      switch(msg){
        case UP:
          rotateTo(NORTH,true);
          delay(100);
          break;
        case DOWN:
          rotateTo(SOUTH,true);
          delay(100);
          break;
        case LEFT:
          rotateTo(WEST,true);
          delay(100);
          break;
        case RIGHT:
          rotateTo(EAST,true);
          delay(100);
          break;
        default:
        digitalWrite(rdLED, HIGH);
        digitalWrite(grLED, HIGH);
        digitalWrite(ylLED, HIGH);
        digitalWrite(blLED, HIGH);
      }
      forward();
      center();

      /* //REVISAR POSICIONES
        for (i =0; i < 4; i++){
          switch(msg){
            case UP:
              obstaculo(UP,i);
            break;
            case DOWN:
              obstaculo(DOWN,i);
            break;
            case LEFT:
              obstaculo(LEFT,i);
            break;
            case RIGHT:
              obstaculo(RIGHT,i);
            break;
          }
          rotate(1);
          delay(100);
        }*/
    delay(1500);
    msg = 0;
  }
  
  void obstaculo(int dir, int i){
  byte OBJ = 0xFF;
  digitalWrite(9,LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  digitalWrite(9, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  tiempo=100;//pulseIn(8, HIGH);
  distancia= int(0.017*tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
  delay(1000);
  if (distancia < 100){
    switch(dir){
      case UP:
      switch(i){
        case 1:
          OBJ = OBJ - B11000000;//norte
        break;
        case 2:
          OBJ = OBJ - B00000011;//oeste
        break;
        case 3:
          OBJ = OBJ - B00110000;//sur
        break;
        case 4:
          OBJ = OBJ - B00001100;//este
        break;
      }
      break;
      case DOWN:
      switch(i){
        case 1:
          OBJ = OBJ - B00110000;//sur
        break;
        case 2:
          OBJ = OBJ - B00001100;//este
        break;
        case 3:
          OBJ = OBJ - B11000000;//norte
        break;
        case 4:
          OBJ = OBJ - B00000011;//oeste
        break;
      }
      break;
      case RIGHT:
        switch(i){
        case 1:
          OBJ = OBJ - B00001100;//este
        break;
        case 2:
          OBJ = OBJ - B11000000;//norte
        break;
        case 3:
          OBJ = OBJ - B00000011;//oeste
        break;
        case 4:
          OBJ = OBJ - B00110000;//sur
        break;
      }
      break;
      case LEFT:
        switch(i){
        case 1:
          OBJ = OBJ - B00000011;//oeste
        break;
        case 2:
          OBJ = OBJ - B00110000;//sur
        break;
        case 3:
          OBJ = OBJ - B00001100;//este
        break;
        case 4:
          OBJ = OBJ - B11000000;//norte
        break;
      }
      break;
    }
  }
  if (i = 4){
    Serial2.write(0x01);
    delay(10);
    Serial2.write(OBJ);
    delay(10);
  }
  }
  
  void rotateTo(byte dir, boolean check){
    if (cur_dir == dir && check == true){
      return;
    }else{
      switch(cur_dir){
        case NORTH:
          switch(dir){
            case WEST:
              rotate(1);
              break;
            case SOUTH:
              rotate(2);
              break;
            case EAST:
              rotate(3);
              break;
          }
          break;
        case EAST:
          switch(dir){
            case NORTH:
              rotate(1);
              break;
            case WEST:
              rotate(2);
              break;
            case SOUTH:
              rotate(3);
              break;
          }
          break;
        case SOUTH:
          switch(dir){
            case EAST:
              rotate(1);
              break;
            case NORTH:
              rotate(2);
              break;
            case WEST:
              rotate(3);
              break;
          }
          break;
        case WEST:
          switch(dir){
            case SOUTH:
              rotate(1);
              break;
            case EAST:
              rotate(2);
              break;
            case NORTH:
              rotate(3);
              break;
          }
      }
      Move(0x0000,0x0000);
      if (check == true){
        cur_dir = dir;
      }
    }
  }
  
  void rotate(byte index){
    switch(index){
      case 1:
        while (Cliffs[2]){
          Move(SPEEDT,0x0001);
          CheckCliffs();          
        }
        while (Cliffs[1]){
          Move(SPEEDT,0x0001);
          CheckCliffs();
 
        }
        while (Cliffs[2]){
          Move(SPEEDT,0x0001);
          CheckCliffs();
        }
        center();
        break;
      case 2:
        while (Cliffs[2]){
          Move(SPEEDT,0xffff);
          CheckCliffs();
        }
        while (Cliffs[1]){
          Move(SPEEDT,0xffff);
          CheckCliffs();
        }
      case 3:
        while (Cliffs[2]){
          Move(SPEEDT,0xffff);
          CheckCliffs();
        }
        while (Cliffs[1]){
          Move(SPEEDT,0xffff);
          CheckCliffs();
        }
    }
  }
  void forward(){
    do{
      if (Cliffs[2]&&!Cliffs[1]){
        Move(SPEEDF,0x00C8);
      }else if (Cliffs[1]&&!Cliffs[2]){
        Move(SPEEDF,0xff38);
      }else if (!Cliffs[1]&&!Cliffs[2]){
        Move(SPEEDF,0x8000);
      }
      CheckCliffs();
    }while(Cliffs[0]&&Cliffs[3]);
    Move(0x0000,0x0000);
  }
  
  void center(){
    while (!Cliffs[0]||!Cliffs[3]){
      Move(SPEEDT,0x8000);
      CheckCliffs();
    }
    while (Cliffs[1]){
      Move(SPEEDT,0xffff);
      CheckCliffs();
    }
    while (!Cliffs[0]||!Cliffs[3]){
      Move(SPEEDT,0x8000);
      CheckCliffs();
    }
    Move(0x0000,0x0000);
  }
  
  void CheckCliffs(){
    unsigned int value;
    value = ReadCliff(LCliff);
    if (value > Thresholds[0]){
      Cliffs[0] = true;
      digitalWrite(ylLED,LOW);
    }else{
      Cliffs[0] = false;
      digitalWrite(ylLED,HIGH);
    }
    value = ReadCliff(LFrontCliff);
    if (value > Thresholds[1]){
      Cliffs[1] = true;
      digitalWrite(grLED,LOW);
    }else{
      Cliffs[1] = false;
      digitalWrite(grLED,HIGH);
    }
    value = ReadCliff(RFrontCliff);
    if (value > Thresholds[2]){
      Cliffs[2] = true;
      digitalWrite(blLED,LOW);
    }else{
      Cliffs[2] = false;
      digitalWrite(blLED,HIGH);
    }
    value = ReadCliff(RCliff);
    if (value > Thresholds[3]){
      Cliffs[3] = true;
      digitalWrite(rdLED,LOW);
    }else{
      Cliffs[3] = false;
      digitalWrite(rdLED,HIGH);
    }
  }
  
  void CalibrateCliffs(){
    Thresholds[0] = ReadCliff(LCliff)/2;
    Thresholds[1] = ReadCliff(LFrontCliff)/2;
    Thresholds[2] = ReadCliff(RFrontCliff)/2;
    Thresholds[3] = ReadCliff(RCliff)/2;
  }
  
  byte CheckBumpers(){
    byte inByte = 0; // incoming serial byte
    Serial3.write((byte)SensorRqst); // requests sensor data packet
    Serial3.write((byte)Bumper); // request bumpers value
    while (Serial3.available() == 0) {
    }
    inByte = Serial3.read();
    return inByte & 0x03;
  }
  
  unsigned int ReadCliff(byte cliff){
    byte inByteH = 0; // incoming serial byte
    byte inByteL = 0; // incoming serial byte
    unsigned int Value = 0;
    Serial3.write((byte)SensorRqst); // requests sensor data packet
    Serial3.write((byte)cliff); // request bumpers value
    while (Serial3.available() == 0) {
    }
    inByteH = Serial3.read();
    while (Serial3.available() == 0) {
    }
    inByteL = Serial3.read();
    Value = (inByteH << 8) + inByteL;
    //Serial.println(Value);
    return Value;
  }
  
  void Move(int vel, int rad){
    Serial3.write((byte)Start); // This command starts the COI.
    Serial3.write((byte)SafeMode); // set mode to safe 
    Serial3.write((byte)Drive);
    Serial3.write((byte)(vel>>8)); // High vel byte
    Serial3.write((byte)vel); // Low vel byte
    Serial3.write((byte)(rad>>8)); // High rad byte
    Serial3.write((byte)rad); // Low rad byte
  }
