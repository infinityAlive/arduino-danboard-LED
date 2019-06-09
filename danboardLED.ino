#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial BLUE_TOOTH_SERIAL(8, 9); // 接收腳位、傳送腳位 -> 對應 HC06 (TX, RX)
const int RED_PIN = 3;
const int GREEN_PIN = 5;
const int BLUE_PIN = 6;
const int COLOR_MAX = 255;
const int RED = 82;
const int ORANGE = 79;
const int YELLOW = 89;
const int GREEN = 71;
const int BLUE = 66;
const int INDIGO = 73;
const int PURPLE = 80;
const String OFF = "OFF";

String ledColorData = "";
int intervalTime = 5000;

void setup() {
  Serial.begin(9600);   // Arduino 起始 bound rate 9600 與 電腦序列埠連接
  Serial.println("Go go! danboardLED");
  BLUE_TOOTH_SERIAL.begin(9600);       // HC-06 藍牙 bound rate

  // 指定 pin 為輸出
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  ledColorData = "R,O,Y,G,B,I,P";
  controlRainbow ();
  ledColorData = "";
  off();
}

void loop() {
  while (BLUE_TOOTH_SERIAL.available() > 0) {
    off();
    ledColorData = "";  // 初始化
    ledColorData = BLUE_TOOTH_SERIAL.readString();
    Serial.println("-->" + ledColorData + "<--");
    off();
  }

  if (ledColorData.length() > 0) {
    if (ledColorData.indexOf(OFF) == -1) {
      // 判斷第一個字元是不是數字
      if (isDigit(ledColorData.charAt(0))) {
        int semicolonIndex = ledColorData.indexOf(';');
        intervalTime = ledColorData.substring(0, semicolonIndex).toInt() * 1000;
        Serial.println("*** interval time = " + String(intervalTime));
        splitledColorData(ledColorData.substring(semicolonIndex + 1));
      } else {
        controlRainbow ();
      }
    }
  }
}

void off()
{
  analogWrite(RED_PIN, COLOR_MAX);
  analogWrite(GREEN_PIN, COLOR_MAX);
  analogWrite(BLUE_PIN, COLOR_MAX);
}

void controlRainbow () {
  char colors[ledColorData.length() + 1];
  ledColorData.toCharArray(colors, sizeof(colors));

  for (int i = 0; i < sizeof(colors); i++) {
    switch (colors[i]) {
      case RED: //'R'
        light(COLOR_MAX, 0, 0);
        break;

      case ORANGE://'O'
        light(COLOR_MAX, 127, 0);
        break;

      case YELLOW://'Y'
        light(COLOR_MAX, COLOR_MAX, 0);
        break;

      case GREEN://'G'
        light(0, COLOR_MAX, 0);
        break;

      case BLUE://'B'
        light(0, 0, COLOR_MAX);
        break;

      case INDIGO://'I'
        light(106, 0, COLOR_MAX);
        break;

      case PURPLE://'P'
        light(136, 32, 240);
        break;
    }
  }
}

void light(int red, int green, int blue) {
  analogWrite(RED_PIN, COLOR_MAX - red);
  analogWrite(GREEN_PIN, COLOR_MAX - green);
  analogWrite(BLUE_PIN, COLOR_MAX - blue);
  delay(intervalTime);
}

void splitledColorData(String paletteData) {
  int tokenIndex = 0;
  /*
     EX: 接收到的資料為 255,12,30;134,5,128;0,255,30
     資料內容為第一組 R,G,B 數值;第二組 R,G,B 數值;第三組 R,G,B 數值
  */
  // 將各組調色盤數值分離出來
  do {
    Serial.println("------ All palette data ------");
    Serial.println(paletteData);

    String uniquePaletteData = "";
    tokenIndex = paletteData.indexOf(';');
    if (tokenIndex != -1) {
      uniquePaletteData = paletteData.substring(0, tokenIndex);
      paletteData = paletteData.substring(tokenIndex + 1);
    } else {
      uniquePaletteData = paletteData;
    }

    if (uniquePaletteData.length() > 0) {
      determineRGB(uniquePaletteData);
    }
  } while (tokenIndex >= 0);
}

void determineRGB(String uniquePaletteData) {
  int firstCommaIndex = uniquePaletteData.indexOf(',');
  int lastCommaIndex = uniquePaletteData.lastIndexOf(',');

  int red = uniquePaletteData.substring(0, firstCommaIndex).toInt();
  int green = uniquePaletteData.substring(firstCommaIndex + 1, lastCommaIndex).toInt();
  int blue = uniquePaletteData.substring(lastCommaIndex + 1).toInt();

  char rgb[20];
  sprintf(rgb, "RGB = %d, %d, %d", red, green, blue);
  Serial.println(rgb);
  light(red, green, blue);
}


