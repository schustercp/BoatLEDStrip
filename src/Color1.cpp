#include <OctoWS2811.h>

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

void christmass(OctoWS2811* pLeds, double intensity)
{
  static uint8_t state = 0;
  uint32_t color1 = 0;
  uint32_t color2 = 0;

  if (intensity > 1.0)
  {
      intensity = 1.0;
  }
  else if (intensity < 0.0)
  {
      intensity = 0.0;
  }

  if(state)
  {
    state = 0;
    color1 = uint32_t(255.0 * intensity) << 16;
    color2 = uint32_t(255.0 * intensity) << 8;
  }
  else
  {
    state = 1;
    color2 = uint32_t(255.0 * intensity) << 16;
    color1 = uint32_t(255.0 * intensity) << 8;
  }

  for (int i = 0; i < pLeds->numPixels(); i += 2) 
  {
    pLeds->setPixel(i, color1);
    pLeds->setPixel(i+1, color2);
  }

  pLeds->show();
  delayMicroseconds(1000000);
}

void fade(OctoWS2811* pLeds, int color, double maxIntensity)
{
  static double intensity = 0;
  static bool direction = true;

  if (maxIntensity > 1.0)
  {
      maxIntensity = 1.0;
  }
  else if (maxIntensity < 0.0)
  {
      maxIntensity = 0.0;
  }

  uint32_t Red   = (color & 0x00FF0000) >> 16;
  uint32_t Green = (color & 0x0000FF00) >> 8;
  uint32_t Blue  =  color & 0x000000FF;

  if(direction)
  {
   intensity += 0.01;
  }
  else
  {
    intensity -= 0.01;
  }

  if(direction && intensity > maxIntensity)
  {
    intensity = maxIntensity;
    direction = false;
  }
  else if(!direction && intensity < 0.0)
  {
    intensity = 0.0;
    direction = true;
  }

  Serial.println(intensity);

  double newRed = double(Red) * intensity;
  double newGreen = double(Green) * intensity;
  double newBlue = double(Blue) * intensity;

  uint32_t displayColor = (uint32_t(newRed) << 16) | (uint32_t(newGreen) << 8) | uint32_t(newBlue);

  for (int i = 0; i < pLeds->numPixels(); i++) 
  {
    pLeds->setPixel(i, displayColor);
  }

  pLeds->show();

  delayMicroseconds(50000);
}