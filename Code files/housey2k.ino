#include <Wire.h>
#include <SPI.h>
#include <String.h>
#include <LiquidCrystal_I2C.h>
#include <MCP4261.h>

// SPI Bus BEGIN
#define MOD_CS1 36
#define MOD_CS2 37
#define MOD_CS3 38
#define MOD_CS4 39
#define MOD_CS5 40

#define P1_CS 15
#define P2_CS 16
// SPI Bus END

// GPIO BEGIN
#define ENTER 3
#define UP 4
#define DOWN 5
#define RIGHT 6
#define LEFT 7

#define CTRL_IN1_OUT1 17
#define CTRL_IN1_OUT2 18
#define CTRL_IN2_OUT1 21
#define CTRL_IN2_OUT2 35
// GPIO END

// BUS BEGIN
#define LCD_ADDR 0x27
#define SDA 8
#define SCL 9

#define MISO 12
#define MOSI 13
#define SCK 14
// BUS END

LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);
MCP4261 pot1(P1_CS, 255, &SPI);
MCP4261 pot2(P2_CS, 255, &SPI);

// scan for I2C devices
void I2C_Scan(void)
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
}

// writes SPI
void spiSendBuffer(const uint8_t *buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    SPI.transfer(buf[i]);
  }
}

// reads SPI
void spiReadBuffer(uint8_t *buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    buf[i] = SPI.transfer(0x00);
  }
}

// detect ports with modules connected
String Module_scan() {
  static const uint8_t detectCmd[] = { 'D','E','T','E','C','T' };
  uint8_t reply[4];

  const uint8_t csList[] = {
    MOD_CS1,
    MOD_CS2,
    MOD_CS3,
    MOD_CS4,
    MOD_CS5
  };

  String output;
  output.reserve(sizeof(csList));

  SPI.beginTransaction(SPISettings(
    500000,
    MSBFIRST,
    SPI_MODE0
  ));

  for (size_t i = 0; i < sizeof(csList); i++) {
    digitalWrite(csList[i], LOW);
    delayMicroseconds(5);

    spiSendBuffer(detectCmd, sizeof(detectCmd));
    delayMicroseconds(20);          // give slave time
    spiReadBuffer(reply, sizeof(reply));

    digitalWrite(csList[i], HIGH);

    output += (memcmp(reply, "HERE", 4) == 0) ? '1' : '0';
    delay(5);
  }

  SPI.endTransaction();
  return output;
}

// writes a debug message to UART and to the LCD during boot
void print(String msg, bool printToLCD_Y3) {
  Serial.println(msg);
  if (!printToLCD_Y3) { return; }
  lcd.setCursor(0, 3);
  lcd.print("                    ");
  lcd.setCursor(0, 3);
  lcd.print(msg);
}

// write/read test for the MCP4261, used to confirm the board is fine
bool RW_MCP4261(MCP4261 pot, int wiper) {
  bool success = false;
  pot.setValue(wiper, 123);
  if (pot.getValueDevice(wiper) == 123) {
    success = true;
  }
  return success;
}

void halt() {
  while (true) {}
}

String Connected_modules;

bool RenderedMenu;
bool RenderedArrow;

int ArrowX;
int ArrowY;
int PrevArrowX;
int PrevArrowY;

String CurrMenu;

bool UpPressed;
bool DownPressed;
bool RightPressed;
bool LeftPressed;

int ArrowRawX;
int ArrowRawY;

void setup(void) {
  Serial.begin(115200);
  print("HouseY2K OS", false);

  Wire.begin(SDA, SCL);
  print("Initialized I2C bus", false);

  I2C_Scan();
  print("Hardcoded LCD addr: #define LCD_ADDR ", false);
  Serial.println(LCD_ADDR);

  lcd.init();
  print("LCD init() done", true);
  lcd.backlight();
  print("LCD backlight() done", true);

  lcd.print("FurModularity DEV FW");
  lcd.setCursor(0, 1);
  lcd.print("HouseY2K OS V0.1");
  lcd.setCursor(0, 2);
  lcd.print("Booting...");
  print("Initializing SPI...", true);
  SPI.begin(SCK, MISO, MOSI, -1);
  delay(100);

  print("Initializing pins...", true);
  pinMode(MOD_CS1, OUTPUT);
  pinMode(MOD_CS2, OUTPUT);
  pinMode(MOD_CS3, OUTPUT);
  pinMode(MOD_CS4, OUTPUT);
  pinMode(MOD_CS5, OUTPUT);

  pinMode(ENTER, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);

  pinMode(CTRL_IN1_OUT1, INPUT_PULLUP);
  pinMode(CTRL_IN1_OUT2, INPUT_PULLUP);
  pinMode(CTRL_IN2_OUT1, INPUT_PULLUP);
  pinMode(CTRL_IN2_OUT2, INPUT_PULLUP);

  delay(100);

  print("POST MCP4261", true);
  delay(100);

  print("INIT MCP4261", true);
  pot1.begin();
  pot2.begin();
  delay(100);

  print("RW MCP4261 IC1 W0", true);
  if (RW_MCP4261(pot1, 0)) {
    print("OK", true);
  } else {
    print("ERROR IC1 W0", true);
    halt();
  }

  delay(100);

  print("RW MCP4261 IC1 W1", true);
  if (RW_MCP4261(pot1, 1)) {
    print("OK", true);
  } else {
    print("ERROR IC1 W1", true);
    halt();
  }

  delay(100);

  print("RW MCP4261 IC2 W0", true);
  if (RW_MCP4261(pot2, 0)) {
    print("OK", true);
  } else {
    print("ERROR IC2 W0", true);
    halt();
  }

  delay(100);

  print("RW MCP4261 IC2 W1", true);
  if (RW_MCP4261(pot2, 1)) {
    print("OK", true);
  } else {
    print("ERROR IC2 W1", true);
    halt();
  }

  delay(100);
  

  print("Scanning modules...", true);
  Connected_modules = Module_scan();

  print("mdet:" + Connected_modules, true);

  delay(1000);
  print("Initializing vars", true);

  RenderedMenu = false;
  CurrMenu = "NULL";

  RenderedArrow = false;
  ArrowX = 0;
  ArrowY = 0;

  PrevArrowX = 0;
  PrevArrowY = 0;

  ArrowRawX = 0;
  ArrowRawY = 0;

  UpPressed = false;
  DownPressed = false;
  RightPressed = false;
  UpPressed = false;
  delay(100);
  print("BootOK LoadOS", true);
}

// renders the initial menu
void RenderMainMenu(String Mods) {
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print(" MODS USB ");
  lcd.setCursor(0, 1);
  lcd.print("MIXER PWR");
  lcd.setCursor(0, 2);
  lcd.print(" WiFi BATT");
  lcd.setCursor(0, 2);
  lcd.print(" BLE ");
}


// draws the arrow to a position on the screen and cleans the old one
void RenderArrow(int X, int Y) {
  RenderedArrow = true;
  lcd.setCursor(PrevArrowX, PrevArrowY);
  lcd.print(" ");
  lcd.setCursor(X, Y);
  lcd.print(">");
  PrevArrowX = X;
  PrevArrowY = Y;
}

// Gets the position to write the arrow on screen, rawpos is the numerical value based on the increments or decrements from left/right buttons
int GetArrowXPos(int rawPos) {
  if (CurrMenu == "MAIN") {
    if (rawPos == 0) {
      return 0;
    } else if (rawPos == 1) {
      return 5;
    } else if (rawPos == 2) {
      return 9;
    }
  }
  return 0;
}

// Gets the position to write the arrow on screen, rawpos is the numerical value based on the increments or decrements from up/down buttons
int GetArrowYPos(int rawPos) {
  /*if (CurrMenu == "MAIN") {
    if (rawPos == 0) {
      return 0;
    } else if (rawPos == 1) {
      return 1;
    } else if (rawPos == 2) {
      return 2;
    }
  }*/
  return rawPos;
}

void loop(void) {

  if (!digitalRead(UP) && !UpPressed) {
    UpPressed = true;
    RenderedArrow = false;
    ArrowRawY++;
  } else if (digitalRead(UP)) {
    UpPressed = false;
  }

  if (!digitalRead(DOWN) && !DownPressed) {
    DownPressed = true;
    RenderedArrow = false;
    ArrowRawY--;
  } else if (digitalRead(DOWN)) {
    DownPressed = false;
  }

  if (!digitalRead(RIGHT) && !RightPressed) {
    RightPressed = true;
    RenderedArrow = false;
    ArrowRawX++;
  } else if (digitalRead(RIGHT)) {
    RightPressed = false;
  }

  if (!digitalRead(LEFT) && !LeftPressed) {
    LeftPressed = true;
    RenderedArrow = false;
    ArrowRawX--;
  } else if (digitalRead(LEFT)) {
    LeftPressed = false;
  }

  if (CurrMenu == "NULL")
  {
    CurrMenu = "MAIN";
    RenderedMenu = true;
    RenderMainMenu(Connected_modules);
  }

  if (!RenderedArrow) {
    RenderArrow(GetArrowXPos(ArrowRawX), GetArrowYPos(ArrowRawY));
  }
}
