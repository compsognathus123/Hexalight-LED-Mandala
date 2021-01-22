/*READMEE
 *    04.08.2020 Poti an A4 hinzugefügt zum Einstellen das bandAdjustment[0] (Bass)
 *                Overallbrightness einstellung max. auf 6500 erhöht
 * 
 * 
 * 
 */



#include <FastLED.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <math.h>

//FastLED.setDither( 0 );

#define DATA_PIN 7
#define NUM_LEDS 216
#define MSGEQ7 A6
#define STROBE_PIN 6
#define RESET_PIN 5
#define ROTARYA 3 //A
#define ROTARYB 4 //B
#define ROTARY_BUTTON 2
#define POTI_PIN A4

//Music-sensitive modes
#define MODE_SPECTRAL_CHILL 10
#define MODE_SPECTRAL 11
#define MODE_SPECTRAL_CONE 12

//Non music-sensitive modes
#define MODE_DAYDREAM 20
#define MODE_ALLON 21

//Color Functions
#define COLOR_RADIAL 30
#define COLOR_SINXY 31
#define COLOR_SPLITX 32
#define COLOR_SPLITY 33


SoftwareSerial bt(11, 12);

CRGB ledsRGB[NUM_LEDS];

long refresh_led_time;
byte standardBrightness = 80;

typedef struct
{
  byte x;
  byte y;
} Coordinate;

Coordinate led_coordinate[NUM_LEDS] = {{58, 3}, {57, 5}, {56, 6}, {55, 8}, {54, 10}, {53, 12}, {52, 13}, {51, 15}, {50, 17}, {49, 19}, {48, 20}, {47, 22}, {46, 24}, {45, 25}, {44, 27}, {43, 29}, {42, 31}, {41, 32},
  {38, 38}, {37, 40}, {36, 42}, {35, 43}, {34, 45}, {33, 47}, {32, 48}, {31, 50}, {30, 52}, {29, 54}, {28, 55}, {27, 57}, {26, 59}, {25, 60}, {24, 62}, {23, 64}, {22, 66}, {21, 67},
  {58, 67}, {57, 65}, {56, 63}, {55, 61}, {54, 60}, {53, 58}, {52, 56}, {51, 55}, {50, 53}, {49, 51}, {48, 49}, {47, 48}, {46, 46}, {45, 44}, {44, 43}, {43, 41}, {42, 39}, {41, 38},
  {38, 32}, {37, 30}, {36, 28}, {35, 26}, {34, 25}, {33, 23}, {32, 21}, {31, 20}, {30, 18}, {29, 16}, {28, 14}, {27, 13}, {26, 11}, {25, 9}, {24, 8}, {23, 6}, {22, 4}, {21, 3},
  {2, 35}, {4, 35}, {6, 35}, {8, 35}, {10, 35}, {12, 35}, {14, 35}, {16, 35}, {18, 35}, {20, 35}, {22, 35}, {24, 35}, {26, 35}, {28, 35}, {30, 35}, {32, 35}, {34, 35}, {36, 35},
  {43, 35}, {45, 35}, {47, 35}, {49, 35}, {51, 35}, {53, 35}, {55, 35}, {57, 35}, {59, 35}, {61, 35}, {63, 35}, {65, 35}, {67, 35}, {69, 35}, {71, 35}, {73, 35}, {75, 35}, {77, 35},
  {78, 38}, {77, 39}, {76, 41}, {75, 43}, {74, 44}, {73, 46}, {72, 48}, {71, 49}, {70, 51}, {69, 53}, {68, 55}, {67, 56}, {66, 58}, {65, 60}, {64, 61}, {63, 63}, {62, 65}, {61, 67},
  {56, 70}, {54, 70}, {52, 70}, {50, 70}, {48, 70}, {46, 70}, {44, 70}, {42, 70}, {40, 70}, {38, 70}, {36, 70}, {34, 70}, {32, 70}, {30, 70}, {28, 70}, {26, 70}, {24, 70}, {22, 70},
  {18, 67}, {17, 65}, {16, 63}, {15, 61}, {14, 60}, {13, 58}, {12, 56}, {11, 55}, {10, 53}, {9, 51}, {8, 49}, {7, 48}, {6, 46}, {5, 44}, {4, 43}, {3, 41}, {2, 39}, {1, 38},
  {1, 32}, {2, 30}, {3, 28}, {4, 26}, {5, 25}, {6, 23}, {7, 21}, {8, 20}, {9, 18}, {10, 16}, {11, 14}, {12, 13}, {13, 11}, {14, 9}, {15, 8}, {16, 6}, {17, 4}, {18, 3},
  {23, 1}, {25, 1}, {27, 1}, {29, 1}, {31, 1}, {33, 1}, {35, 1}, {37, 1}, {39, 1}, {41, 1}, {43, 1}, {45, 1}, {47, 1}, {49, 1}, {51, 1}, {53, 1}, {55, 1}, {57, 1},
  {61, 3}, {62, 4}, {63, 6}, {64, 8}, {65, 9}, {66, 11}, {67, 13}, {68, 14}, {69, 16}, {70, 18}, {71, 20}, {72, 21}, {73, 23}, {74, 25}, {75, 26}, {76, 28}, {77, 30}, {78, 32}
};


//get LED's by panel
byte innerPanelIndex[6] = {17, 90, 53, 18, 89, 54};
byte outerPanelIndex[6] = {198, 108, 126, 144, 162, 180};

//Music-sensitive Stuff
boolean music_sensitive;
byte bygoneBandValue[7][18];
int bandValue[7];
int smoothBassValue;

//App-configurable Bytes
byte brightness;
//Bass-Adjustment by poti, 5th band too low in general
byte bandAdjustment[7] = {128, 128, 128, 128, 210, 128, 128};

//Color Parameters
byte color1 = 100; //128
byte color2 = 230; //220
byte color_speed = 200;
byte color_size = 200;
byte color_function = COLOR_RADIAL;

//Brightness/Wave Parameters
byte wave_speed = 245;
byte wave_size = 10;

byte mode = MODE_SPECTRAL;

byte bt_message[8];

//Beat Detection
byte maxBassVal;
bool inKick;

int bpm_1;
int bpm_2;
long bpm_1_start;
long bpm_2_start;

long newMessage;
byte n_bytes = 0;

volatile int rotary_val;
int rotary_old;
long rotary_time;

byte selected_mode;

/*
  Arduino color1 color2 colorSpeed colorSize waveSize waveSpeed colorFct mode

  Android color1 color2 colorSpeed colorSize mode waveSpeed waveSize colorFct
    
 */

byte default_modes[5][8] = {{ 0,255,150,128,128,128,30,10},
                            {0, 255, 128, 128, 128, 100, 30, 20},
                            {128, 220, 120, 180, 10, 128, 30, 20},
                            {215, 130, 128, 128, 128, 128, 32, 20},
                            {128, 150, 50, 50 ,50 ,50, 30,21}};
             

void setup() 
{

  Serial.begin(9600);
  bt.begin(9600);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(ledsRGB, NUM_LEDS);

  pinMode(MSGEQ7, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  //pinMode(POTI_PIN, INPUT);
  pinMode(ROTARY_BUTTON, INPUT);
  
  pinMode(ROTARYA, INPUT);
  pinMode(ROTARYB, INPUT);
  digitalWrite(ROTARYA, HIGH);
  digitalWrite(ROTARYB, HIGH);

  analogReference(DEFAULT);

  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH);
  
  pinMode(ROTARY_BUTTON, INPUT_PULLUP);  
  pinMode(POTI_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(ROTARYA), RotaryInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_BUTTON), ButtonInterrupt, FALLING);

  //EEPROM WRITE
 
 /* for(int i = 60; i < 1024; i++)
  {
    EEPROM.write(i,0);
  }
  
 for(int i = 0; i < 5; i++)
  {
    EEPROM.write(10 + 10 * i, 255);
    for(int j = 1; j < 9; j++)
    {
      EEPROM.write(10 + i * 10 + j, default_modes[i][j-1]);
    }
  }*/

  
   

  for (int i = 0; i < NUM_LEDS; i++)
  {
    ledsRGB[i] = CHSV(0, 255, standardBrightness);
  }

  for (int i = 0; i < sizeof(bt_message); i++)
  {
    bt_message[i] = 0;    
  }
  
  selected_mode = EEPROM.read(0);
  
  rotary_val = EEPROM.read(1);  
  rotary_old = rotary_val;
  
  loadModeFromEE(selected_mode);
   // mode = MODE_SPECTRAL_CONE;

  FastLED.show();
  for(int i = 0; i < 80; i++)
  {
    Serial.print(i);
    Serial.print("\t");
    Serial.println(EEPROM.read(i));
  }



}

void ButtonInterrupt()
{
   selected_mode++;
    if(EEPROM.read(10 + selected_mode * 10) == 0)
    {
      selected_mode = 0;
    }
    Serial.println(selected_mode);
    loadModeFromEE(selected_mode);
}

void loadModeFromEE(int index)
{
    EEPROM.write(0, index);
  index *=10;
  index += 10;
    color1 = EEPROM.read(index + 1);
    color2 = EEPROM.read(index + 2);
    color_speed = EEPROM.read(index + 3);
    color_size = EEPROM.read(index + 4);
    wave_size = EEPROM.read(index + 5);
    wave_speed = EEPROM.read(index + 6);
    color_function = EEPROM.read(index + 7);
    mode = EEPROM.read(index + 8);


    
}

void loop() 
{
 //mode = MODE_SPECTRAL_CONE;
  
  if(rotary_time + 10000 < millis() && rotary_val != rotary_old){
    rotary_old = rotary_val;
    EEPROM.write(1, rotary_val);
    Serial.println("EEPROM Rotary written!");
  }

  if (handleBluetoothCommunication())
  {
    color1 = bt_message[0];
    color2 = bt_message[1];
    color_speed = bt_message[2];
    color_size = bt_message[3];
    wave_size = bt_message[4];
    wave_speed = bt_message[5];
    color_function = bt_message[6];
    mode = bt_message[7];

    for(int i = 0; i < sizeof(bt_message); i++)
    {
      EEPROM.write(10, 255);
      EEPROM.write(11 + i, bt_message[i]);
    }

    Serial.print(color1);
      Serial.print("\t");
    Serial.print(color2);
      Serial.print("\t");
    Serial.print(color_speed);
      Serial.print("\t");
    Serial.print(color_size);
      Serial.print("\t");
    Serial.print(wave_speed);
      Serial.print("\t");
    Serial.print(wave_size);
      Serial.println("\t");

    for (int i = 0; i < sizeof(bt_message); i++)
    {
      Serial.print(bt_message[i]);
      Serial.print("\t");
    }
    Serial.print(sizeof(bt_message));
    Serial.println("## updated");
  }

  music_sensitive = (mode >= 10 && mode < 20);

  if (millis() - refresh_led_time > 30)
  {
                                               // long watch = millis();
    refresh_led_time = millis();
    FastLED.show();

    applyFunctions();

    if (music_sensitive)
    {
      readMSGEQ7();
      correctBandValues();
      saveCurrentBandValues();
    }
                                                //Serial.println(millis() - watch);
  }


}

void RotaryInterrupt()
{
  if(digitalRead(ROTARYB) != digitalRead(ROTARYA))
  {
    if(rotary_val < 254){
      rotary_val+=2;
    }
  }else{
     if(rotary_val >= 2){
      rotary_val-=2;
    }
  }
   rotary_time = millis();
  //Serial.println(rotary_val);
}

boolean handleBluetoothCommunication()
{
  if(bt.available())
  {
    if(millis() > newMessage + 500 || n_bytes > 8){
       n_bytes = 0;
       newMessage = millis();
    }
    bt_message[n_bytes] = bt.read();
   // Serial.print(bt_message[n_bytes]);
    //Serial.print("\t");
    n_bytes++;
  }  

  if(n_bytes == 8)
  {
    n_bytes = 0;
    Serial.println("");
    return true;
  }else{
    return false;    
  }

 /* while (bt.available())
  {
    bt_message[n_bytes] = bt.read();
    n_bytes++;
  }

  if (n_bytes < 8)
  {
    return false;
  }
  else
  {
  
    return true;
  }*/
}

void applyFunctions()
{
  int f = 0;

  //Calc color-function parameters
  float relColorWidth = ((color2 - color1) / 255.0);
  byte colorOffset = (color1 + color2) / 2 - relColorWidth * 128;
  byte phi_color = millis() / map(color_speed, 0, 255, 150, 0);
  byte color_size_param = map(color_size, 0, 255, 100, 0);

  //Calc brightness-function parameters

  byte wave_size_param = map(wave_size, 0, 255, 255, 0);
  byte phi_wave = millis() / map(wave_speed, 0, 255, 25, 0);


  byte hue;
  byte brightness;
  byte saturation = 255;

  //byte poti_brightness = map(rotary_val, 0, 1024, 0, 255);
  byte poti_brightness = map(rotary_val, 0, 256, 0, 255);

  double phi_cone = millis()/100;


  for (int i = 0; i < NUM_LEDS; i++)
  {
   // long watch = millis();

    hue = getColorByFunction(led_coordinate[i].x, led_coordinate[i].y, relColorWidth, colorOffset, phi_color, color_size_param);
    //f += millis() - watch;

    if (mode == MODE_DAYDREAM)
    {
      brightness = getBrightnessByFunction(led_coordinate[i].x, led_coordinate[i].y, phi_wave, wave_size_param);
      brightness = brightness * (poti_brightness/255.0);      
    }
    else if (mode == MODE_ALLON)
    {
      brightness = 255;
    }
    else if (mode == MODE_SPECTRAL || mode == MODE_SPECTRAL_CHILL)
    {
      brightness = getBrightnessByMusic(i);

      if (i >= NUM_LEDS / 2)
      {
        saturation = 100;
      }
      else
      {
        saturation = 255;
      }
    }
    else if(mode == MODE_SPECTRAL_CONE)
    {
        double alpha;
        double x = led_coordinate[i].x-40;
        double y = led_coordinate[i].y-36;
        
        if(x > 0)
        {
          alpha = atan(x/y);
        }
        else
        {
          if(y > 0)
          {
            alpha = atan(x/y) + PI;
          }
          else
          {
            alpha = atan(x/y) - PI;
          }
        }
        
        hue = (sin(alpha + phi_cone)+1)*127;
        brightness = 255;
        
        /*Serial.print(alpha);
        Serial.print("\t");
        Serial.println(hue);*/
    }

    if (brightness < 20) brightness = 0;
    
    ledsRGB[i].setHSV(hue, saturation, brightness);

  }

}

//Nicht Benutzt!! kann entfernt werden.
bool checkForBeat()
{
  if (bandValue[0] > maxBassVal)
  {
    maxBassVal = bandValue[0];
  }

  if (bandValue[0] < maxBassVal - 10)
  {
    inKick = false;
  }
  else if (bandValue[0] > maxBassVal - 10 && !inKick)
  {
    inKick = true;
    return true;
  }

  return false;

}

void saveCurrentBandValues()
{
  for (int i = 0; i < 7; i++)
  {
    for (int j = 17; j > 0; j--)
    {
      bygoneBandValue[i][j] = bygoneBandValue[i][j - 1];
    }

    bygoneBandValue[i][0] = bandValue[i];
  }

}

void readMSGEQ7()
{
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);

  for (int i = 0; i < 7; i++)
  {
    digitalWrite(STROBE_PIN, LOW);
    delayMicroseconds(30);
    bandValue[i] = analogRead(MSGEQ7);
    digitalWrite(STROBE_PIN, HIGH);
  }

  /*for(int i = 0; i < 7; i++){
    Serial.print(bandValue[i]);
    Serial.print("\t");
  }
  Serial.println();*/


}

byte getBrightnessByMusic(int led_index)
{
  int panel_number = led_index / 18;
  int n = led_index - panel_number * 18;

  switch (panel_number)
  {
    case 0: return bygoneBandValue[panel_number + 1][17 - n];
    case 1: return bygoneBandValue[panel_number + 1][n];
    case 2: return bygoneBandValue[panel_number + 1][17 - n];
    case 3: return bygoneBandValue[panel_number + 1][n];
    case 4: return bygoneBandValue[panel_number + 1][17 - n];
    case 5: return bygoneBandValue[panel_number + 1][n];

    default:

      if (mode == MODE_SPECTRAL_CHILL)
      {
        return smoothBassValue;
      }
      else
      {
        return bandValue[0];
      }

  }
}

byte getBrightnessByFunction(byte xPos, byte yPos, byte phi, byte wave_size_param)
{
  //Center Mandala
  int x = (xPos - 39);
  int y = (yPos - 35);

  byte dist_from_zero;

  if (x > 0)
  {
    if (y > 0)
    {
      dist_from_zero = y + x;
    }
    else
    {
      dist_from_zero = - y + x;
    }
  }
  else
  {
    if (y > 0)
    {
      dist_from_zero = y + (-x);
    }
    else
    {
      dist_from_zero = (-y) + (-x);
    }
  }

  byte brightness =  sin8(dist_from_zero * 0.04 * wave_size_param  - phi); //RADIAL

  return brightness;
}

byte getColorByFunction(byte xPos, byte yPos, float relColorWidth, byte colorOffset, byte phi, byte color_size_param)
{
  int x = (xPos - 39);
  int y = (yPos - 35);

  byte hue;

  switch (color_function)
  {
    case COLOR_SINXY: hue = colorOffset + relColorWidth * sin8(x * y * 0.00139 * color_size_param  - phi); //SINXY
      break;

    case COLOR_RADIAL:
      byte dist_from_zero;

      if (x > 0)
      {
        if (y > 0)
        {
          dist_from_zero = y + x;
        }
        else
        {
          dist_from_zero = - y + x;
        }
      }
      else
      {
        if (y > 0)
        {
          dist_from_zero = y + (-x);
        }
        else
        {
          dist_from_zero = (-y) + (-x);
        }
      }
      hue = colorOffset + relColorWidth * sin8(dist_from_zero * 0.04 * color_size_param  - phi); //RADIAL

      break;

    case COLOR_SPLITX:
      if (x > 0)
      {
        hue = + relColorWidth * 255 + colorOffset;
      }
      else
      {
        hue =  colorOffset;
      }
      break;

    case COLOR_SPLITY:
      if (y > 0)
      {
        hue = + relColorWidth * 255 + colorOffset;
      }
      else
      {
        hue =  colorOffset;
      }
      break;

  }


  return hue;
}

void correctBandValues()
{
  //READ POTI/ROTARY
  //float mapped_poti = map(rotary_val, 0, 1024, 0, 5000);

  float mapped_poti = map(rotary_val, 0, 256, 1484, 6500);
  float overallBrightness = mapped_poti / 1000;

  bandAdjustment[0] = map(analogRead(POTI_PIN), 0, 1023, 255, 0);

  // Do correction
  for (int i = 0; i < 7; i++)
  {
    bandValue[i] = (bandAdjustment[i] / 255.0) * (float)bandValue[i] * 2;
    bandValue[i] *= overallBrightness;
    bandValue[i] = map(bandValue[i], 0, 5 * 1024, 0, 255);

    if (i == 0 && bandValue[0] < standardBrightness) bandValue[0] = standardBrightness; //IF chillmode = false --> outerPanel at standardBrightness

    if (bandValue[i] < standardBrightness)
    {
      bandValue[i] = 0;

    }
    else if (bandValue[i] > 255)
    {
      bandValue[i] = 255;
    }
  }

  for(int i = 0; i < 7; i++){
    Serial.print(bandValue[i]);
    Serial.print("\t");
  }
  Serial.println();
  
  //Smooth the bass
  if (bandValue[0] > smoothBassValue)
  {
    smoothBassValue += bandValue[0] / 5;
  }
  else
  {
    smoothBassValue -= 5;
  }

  if (smoothBassValue > 255)
  {
    smoothBassValue = 255;
  }
  else if (smoothBassValue < standardBrightness)
  {
    smoothBassValue = standardBrightness;
  }
}
