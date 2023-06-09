/*#include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <SoftwareSerial.h>

  SoftwareSerial HC05(12, 13);
*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#define TFT_CS 10
#define TFT_RST 8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#include <SoftwareSerial.h>

SoftwareSerial master(4, 5);
/*
   CARACTERISTIQUE OLED
   TAILLE :
   x : 0 160
   y : 0 80
   (a l'envers)

*/

// Constants
#define NUM_JOY 2
#define MIN_VAL 0
#define MAX_VAL 1023

// Parameters
const int joyPin[2] = {A0, A1};
const int joyBtn = 2;
const int joyOffset = 0;

// Variables
int joyVal[NUM_JOY] = {0, 0};

int pixel[200][2];
int num_pixel = 0;

int x = 0, y = 0;

int tempB = 1000;
int count = 0;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    master.begin(9600);
    tft.initR(INITR_BLACKTAB);   // Init ST7735S chip, black tab
    tft.initR(INITR_MINI160x80); // Init ST7735S mini display
    tft.fillScreen(ST77XX_BLACK);
    tft.invertDisplay(true);

    for (int i = 0; i < NUM_JOY; i++)
        pinMode(joyPin[i], INPUT);
    pinMode(joyBtn, INPUT_PULLUP);
}

void loop()
{
    if (master.available())
    {
        int data = master.read();
        Serial.print("data");
        Serial.println(data);
        if (data <= 150 && data >= 70)
        {
            y = data - 70;
            Serial.print("y : ");
            Serial.println(y);
        }
        else
        {
            y = y;
        }
        if (data <= 60)
        {
            x = data;
            Serial.print("x : ");
            Serial.println(x);
        }
        else
        {
            x = x;
        }
    }

    // pixel[num_pixel][0] = x;
    // Serial.print("x");
    // Serial.println(x);
    // pixel[num_pixel][1] = y;
    // Serial.print("y");
    // Serial.println(y);
    if (x >= 0 && x <= 80 && y >= 0 && y <= 160)
    {
        tft.drawPixel(y, x, ST77XX_GREEN);
        // num_pixel++;
    }

    if (num_pixel == 199)
    {
        num_pixel = 0;
        for (int i = 0; i < 199; i++)
        {
            pixel[num_pixel][0] = 0;
            pixel[num_pixel][1] = 0;
        }
    }

    readJoystick();

    joyVal[0] = map(joyVal[0], 0, 1023, 50, 130);
    joyVal[1] = map(joyVal[1], 0, 1023, 190, 210);
    master.write(joyVal[0]);
    master.write(joyVal[1]);
    tft.drawPixel(40, 80, ST77XX_GREEN);
}

int joyRawToPhys(int raw)
{ /* function joyRawToPhys */
    ////Joystick conversion rule
    int phys = map(raw, MIN_VAL, MAX_VAL, 0 + joyOffset, 20 + joyOffset) - joyOffset;
    return phys;
}

void readJoystick()
{ /* function readJoystick */
    ////Test routine for Joystick
    for (int i = 0; i < NUM_JOY; i++)
    {
        joyVal[i] = analogRead(joyPin[i]);
    }
    for (int i = 0; i < NUM_JOY; i++)
    {
        // Serial.print(F("Conv")), Serial.print(i);
        // Serial.print(F(" : "));
        // Serial.println(joyRawToPhys(joyVal[i]));
    }
    if (!digitalRead(joyBtn))
    {
        // Serial.println(F("Joy Button pressed"));
        centre();
        master.write(250);
        delay(500);
    }
    num_pixel++;
    pixel[num_pixel][0] = joyRawToPhys(joyVal[0]);
    pixel[num_pixel][1] = joyRawToPhys(joyVal[1]);
    tft.drawPixel(pixel[num_pixel][0], pixel[num_pixel][1], ST77XX_GREEN);
}

void dezoom()
{
    tft.fillScreen(ST77XX_BLACK);
    for (int nb = 0; nb < num_pixel; nb++)
    {
        pixel[nb][0] = ((pixel[nb][0] - 40) / 1.3) + 40;
        pixel[nb][1] = ((pixel[nb][1] - 80) / 1.6) + 80;
        tft.drawPixel(pixel[nb][0], pixel[nb][1], ST77XX_GREEN);
    }
}

void zoom()
{
    tft.fillScreen(ST77XX_BLACK);
    for (int nb = 0; nb < num_pixel; nb++)
    {
        pixel[nb][0] = ((pixel[nb][0] - 40) * 1.3) + 40;
        pixel[nb][1] = ((pixel[nb][1] - 80) * 1.6) + 80;
        if (pixel[nb][1] > 180)
            pixel[nb][1] = 180;
        if (pixel[nb][0] > 80)
            pixel[nb][0] = 80;
        tft.drawPixel(pixel[nb][0], pixel[nb][1], ST77XX_GREEN);
    }
}

void centre()
{
    tft.fillScreen(ST77XX_BLACK);
    int maxx = pixel[0][0];
    int maxy = pixel[0][1];
    int minx = pixel[0][0];
    int miny = pixel[0][1];
    for (int nb = 0; nb < num_pixel; nb++)
    {
        if (maxx < pixel[nb][0])
        {
            maxx = pixel[nb][0];
        }
        if (maxy < pixel[nb][1])
        {
            maxy = pixel[nb][1];
        }
        if (minx > pixel[nb][0])
        {
            minx = pixel[nb][0];
        }
        if (miny > pixel[nb][1])
        {
            miny = pixel[nb][1];
        }
    }
    int diffx = (maxx - minx) / 2; // 40 = (80-0)/2
    int diffy = (maxy - miny) / 2; // 20 = (40-0)/2

    diffx = (tft.width() / 2) - diffx;
    diffy = (tft.height() / 2) - diffy;
    // Serial.print(maxx);
    // Serial.print("\t");
    // Serial.print(minx);
    // Serial.print("\t");
    // Serial.print(maxy);
    // Serial.print("\t");
    // Serial.print(miny);
    // Serial.print("\t");
    // Serial.println(num_pixel);
    for (int nb = 0; nb < num_pixel; nb++)
    {
        pixel[nb][0] = pixel[nb][0] + diffx;
        pixel[nb][1] = pixel[nb][1] + diffy;
        // pixel[nb][0] =  map(pixel[nb][0], minx, maxx, tft.height() / 2 - diffx , tft.height() / 2 + diffx);
        // pixel[nb][1] =  map(pixel[nb][1], miny, maxy, tft.width() / 2 - diffy, tft.width() / 2 + diffy);
        tft.drawPixel(pixel[nb][0], pixel[nb][1], ST77XX_GREEN);
    }
}
