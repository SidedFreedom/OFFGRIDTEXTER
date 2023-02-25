#include <RH_RF95.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <RotaryEncoder.h>

//CUSTOM TIMER
int i = 1000;
int j = 1000;

Adafruit_SH1107 oled = Adafruit_SH1107(64, 128, &Wire);

#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
//#define LED      13

#define RFM95_CS      8
#define RFM95_INT     7
#define RFM95_RST     4


#define RF95_FREQ 915.0
bool recv = false;
bool sent = false;
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define PIN_IN1 19
#define PIN_IN2 20
#define SCB 21
#define ROTARYSTEPS 1
#define ROTARYMIN 0
#define ROTARYMAX 27
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

bool  abc = true;
bool  ABC = false;
bool  num = false;
const char lowercase[] PROGMEM = {"_abcdefghijklmnopqrstuvwxyz*"};
const char uppercase[] PROGMEM = {"_ABCDEFGHIJKLMNOPQRSTUVWXYZ*"};
const char numcase[] PROGMEM = {".,0123456789:;'/@#$%&()+=?!*"};
String muneco = "";
String recvd = "";
int lastPos = 0;
int newPos = 0;
int mlen = 0;

char buffer[20];
const char msgrecv[] PROGMEM = {"NewMSG"};
const char msgsent[] PROGMEM = {"MSG Sent!"};
const char counter[] PROGMEM = {"/95\n"};
const char abcmode[] PROGMEM = {"abc"};
const char ABCmode[] PROGMEM = {"ABC"};
const char nummode[] PROGMEM = {"123"};

const char *const string_table[] PROGMEM = {msgrecv,msgsent,counter,abcmode,ABCmode,nummode};

void showme()
{
i = 10000;
oled.clearDisplay();
oled.display();
oled.setCursor(0,0);
//oled.print(muneco);
//oled.display();
mlen = muneco.length();
if(sent == false)
    {
    j=1000;
    oled.print(mlen);
    oled.println(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[2]))));
    oled.print(muneco);
    if(abc == true){oled.print(char(pgm_read_byte_near(lowercase+newPos))); oled.setCursor(110,55); oled.println(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[3]))));}
    if(ABC == true){oled.print(char(pgm_read_byte_near(uppercase+newPos))); oled.setCursor(110,55); oled.println(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[4]))));}
    if(num == true){oled.print(char(pgm_read_byte_near(numcase+newPos))); oled.setCursor(110,55); oled.println(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[5]))));}
    }
if(sent == true){oled.setCursor(35,30);oled.println(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[1]))));sent=false;}
if(recv == true){oled.setCursor(65,0);oled.println(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[0]))));}
oled.display();
}


void setup() {

//  Serial.begin(9600);
//  while (!Serial) { delay(1); } 

  oled.begin(0x3C, true);  //can change to 0x3D if you solder the jumper on the OLED feather
  oled.clearDisplay();
  oled.display();
  oled.setRotation(1);

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
 // pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.println(F("Feather LoRa RX/TX Test!"));
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println(F("LoRa radio init failed"));
    //Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println(F("LoRa radio init OK!"));

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println(F("setFrequency failed"));
    while (1);
  }

  rf95.setTxPower(23, false);

  oled.setTextSize(1);
  oled.setTextColor(SH110X_WHITE);
  
encoder.setPosition(0 / ROTARYSTEPS);
pinMode(SCB, INPUT_PULLUP);

muneco.reserve(96);
recvd.reserve(96);
  delay(500);
showme();
}





void loop() {

for (i = 1000; i > 0; i--)
  {
//Serial.println(i);
////////////////////
  if (rf95.available()) {
    // Should be a message for us now   
    uint8_t buf[96];
    uint8_t len = sizeof(buf);
    
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      recvd = ((char*)buf);  
    }
    else
    {
      Serial.println(F("Receive failed"));
    }
    
    //RH_RF95::printBuffer("Rcvd: ", (char*)buf, len);
    recv = true;
    showme();
  }
////////////////////



////////////////////////////////////////////
if (!digitalRead(BUTTON_B))
  {
delay(250);
oled.clearDisplay();
oled.display();
oled.setCursor(0,0);
oled.print(recvd);
oled.display();
recvd = "";
recv = false;

  }
////////////////////////////////////////////
////////////////////////////////////////////
if (!digitalRead(BUTTON_C))
  {
    delay(250);
    //char data[mlen+1] = {muneco.c_str()};
    
    //rf95.send((uint8_t *)data, sizeof(data));
    rf95.send((uint8_t *)muneco.c_str(), muneco.length()+1);
    rf95.waitPacketSent();
    sent = true;
    muneco = "";
showme();
  }
////////////////////////////////////////////



//////////////////////////////////////////////////////
    encoder.tick();
  newPos = encoder.getPosition() * ROTARYSTEPS;

  if (newPos < ROTARYMIN) {
    encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
    newPos = ROTARYMIN;

  } else if (newPos > ROTARYMAX) {
    encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
    newPos = ROTARYMAX;
  } // if

  if (lastPos != newPos) {
    lastPos = newPos;

showme();  
  }

if(digitalRead(SCB) == LOW && mlen < 96)
{
delay(175);
if(mlen > 0 && newPos == 27)
    {muneco.remove(mlen-1);}
else if (mlen <95 && newPos != 27)
  {
    
    if(abc == true && newPos != 0){muneco += char(pgm_read_byte_near(lowercase+newPos));}
    if(ABC == true && newPos != 0){muneco += char(pgm_read_byte_near(uppercase+newPos));}
    if(abc == true && newPos == 0){muneco += " ";}
    if(ABC == true && newPos == 0){muneco += " ";}
    if(num == true){muneco += char(pgm_read_byte_near(numcase+newPos));}
    
  }
  mlen = muneco.length();

showme();
  }
//////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
if(digitalRead(BUTTON_A) == LOW && abc == true && ABC == false && num == false)
{abc = false; ABC = true; num = false; delay(300);showme();}
if(digitalRead(BUTTON_A) == LOW && abc == false && ABC == true && num == false)
{abc = false; ABC = false; num = true; delay(300);showme();}
if(digitalRead(BUTTON_A) == LOW && abc == false && ABC == false && num == true)
{abc = true; ABC = false; num = false; delay(300);showme();}

//if(digitalRead(BUTTON_B) == LOW)
//{encoder.setPosition(0 / ROTARYSTEPS); newPos = 0; lastPos = 0; muneco = "";  delay(300); showme(); }
///////////////////////////////////////////////////////////////  
  } //end timer
  
i = 1000;
j--;
      if(j < 1)
      {
      oled.clearDisplay();
      oled.display();
      j=1000;
      }

}
