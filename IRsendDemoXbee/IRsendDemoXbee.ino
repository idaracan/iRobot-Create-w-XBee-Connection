/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

  #define UP 0x66
  #define DOWN 0x26
  #define LEFT 0x56
  #define RIGHT 0x16

byte command;

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(13, OUTPUT);
}
int a;
byte msg;
byte ab[] = {0,0,0,0,0,0,0,0,0,0};  
void loop() {
  //MANDA MENSAJE
  while (Serial.available() == 0) {
  }
  command = Serial.read();
  if (command == 117){ //u  
        Serial.write("UP");
        Serial3.write(UP);
  } else if (command == 100){ //d
//    Serial.println(DOWN,HEX);
        Serial.write("DOWN");
        Serial3.write(DOWN);
  } else if (command == 108){ //l
//    Serial.println(LEFT,HEX); 
        Serial.write("LEFT");
        Serial3.write(LEFT);
  } else if (command == 114){ //r;
//    Serial.println(RIGHT,HEX); 
        Serial.write("RIGHT");
        Serial3.write(RIGHT);
  }
  //RECIBE MENSAJE
/**/  int c=0;    
    do{
      digitalWrite(13,HIGH);
      while (Serial3.available() == 0) {
      }
      digitalWrite(13,LOW);
      ab[c]=Serial3.read();
      Serial.print(ab[c],HEX);
      Serial.print("  ");
      Serial.println(c);
      c++;
    }while (c < 10);
    msg= ab[8];
  switch(msg){
    case 0x01:
      Serial.print(msg,HEX);
    break;
     default:
     break; 
  }
  delay(100);
}
