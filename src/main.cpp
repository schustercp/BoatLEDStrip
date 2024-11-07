#include <OctoWS2811.h>
#include <AlaLedRgb.h>

const int ledsPerStrip = 150;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);
AlaLedRgb rgbStrip;

AlaSeq seq[] =
{
  { ALA_OFF,            1000, 2000, alaPalNull },
  { ALA_ON,             1000, 2000, alaPalRgb },
  { ALA_SPARKLE,        1000, 9000, alaPalRgb },
  { ALA_CYCLECOLORS,    3000, 6000, alaPalRgb },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_FADECOLORSLOOP, 3000, 6000, alaPalRgb },
  { ALA_SPARKLE2,       1000, 6000, alaPalRgb },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_SPARKLE2,       1000, 6000, alaPalFire },
  { ALA_PIXELSMOOTHSHIFTRIGHT, 16000, 2000, alaPalRgb },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_MOVINGBARS,     3000, 16000, alaPalRgb },
  { ALA_COMET,          3000, 6000, alaPalRgb },
  { ALA_COMETCOL,       3000, 6000, alaPalRgb },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_GLOW,           3000, 6000, alaPalRgb },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_FIRE,           1000, 6000, alaPalFire },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_BOUNCINGBALLS,  1000, 6000, alaPalRgb },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_BUBBLES,        1000, 6000, alaPalRainbow },
  { ALA_ENDSEQ }
};

int data;
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;

void setup() 
{
  // Start Serial Stream
  Serial.begin(115200);

  leds.begin();
  leds.show();

  rgbStrip.initWS2811(&leds);
  
  rgbStrip.setBrightness(AlaMax(0.9, 0.9, 0.9));

  rgbStrip.setAnimation(seq);
}

void christmass(OctoWS2811* pLeds, double intensity);
void fade(OctoWS2811* pLeds, int color, double maxIntensity);

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

void colorWipe(int color, int wait)
{
  for (int i=0; i < leds.numPixels(); i++) 
  {
    leds.setPixel(i, color);

    if(wait > 0)
    {
      leds.show();
      delayMicroseconds(wait);
    }
  }

  if(wait <= 0)
  {
    leds.show();
  }
}

void recvWithEndMarker() 
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newData == false) 
  {
    rc = Serial.read();

    if (rc != endMarker) 
    {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) 
      {
        ndx = numChars - 1;
      }
    }
    else 
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void loop() 
{
  rgbStrip.runAnimation();
}
