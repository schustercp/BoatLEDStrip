#include <OctoWS2811.h>
#include <AlaLedRgb.h>
#include <CommandParser.h>

typedef CommandParser<16, 6, 10, 32, 64> MyCommandParser;

// template<size_t COMMANDS = 16, size_t COMMAND_ARGS = 6, size_t COMMAND_NAME_LENGTH = 10, size_t COMMAND_ARG_SIZE = 32, size_t RESPONSE_SIZE = 64>
// class CommandParser

MyCommandParser parser;

const int ledsPerStrip = 150;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

uint8_t HeiheiPinList[4] = {2, 14, 6, 20};

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, 4, HeiheiPinList);
AlaLedRgb rgbStrip;
// animation, speed, duration, AlaPalette
AlaSeq default_seq[] =
    {
        {ALA_OFF, 1000, 1000, alaPalNull},
        {ALA_ON, 1000, 15000, alaPalWhite},
        {ALA_CYCLECOLORS, 6000, 30000, alaPalEaster},
        {ALA_FADECOLORSLOOP, 6000, 30000, alaPalEaster},
        {ALA_SPARKLE, 2000, 30000, alaPalEaster},
        {ALA_SPARKLE, 2000, 30000, alaPalParty},
        {ALA_SPARKLE2, 2000, 30000, alaPalParty},
        {ALA_SPARKLE2, 2000, 30000, alaPalEaster},
        {ALA_MOVINGBARS, 6000, 32000, alaPalEaster},
        {ALA_COMET, 6000, 30000, alaPalEaster},
        {ALA_COMETCOL, 6000, 30000, alaPalEaster},
        {ALA_GLOW, 6000, 30000, alaPalEaster},
        {ALA_FIRE, 2000, 30000, alaPalParty},
        {ALA_PLASMA, 2000, 30000, alaPalParty},
        {ALA_BOUNCINGBALLS, 2000, 30000, alaPalEaster},
        {ALA_BUBBLES, 2000, 30000, alaPalRainbow},
        {ALA_LARSONSCANNER, 2000, 30000, alaPalRainbow},
        {ALA_LARSONSCANNER2, 2000, 30000, alaPalRainbow},
        {ALA_ENDSEQ}};

// #define ALA_BLINK 103
// #define ALA_BLINKALT 104
// #define ALA_STROBO 107
// #define ALA_PIXELSHIFTRIGHT 201
// #define ALA_PIXELSHIFTLEFT 202
// #define ALA_PIXELBOUNCE 203
// #define ALA_BARSHIFTRIGHT 231
// #define ALA_BARSHIFTLEFT 232
// #define ALA_MOVINGBARS 241
// #define ALA_MOVINGGRADIENT 242
// #define ALA_LARSONSCANNER 251
// #define ALA_LARSONSCANNER2 252
// #define ALA_FADEIN 301
// #define ALA_FADEOUT 302
// #define ALA_FADEINOUT 303
// #define ALA_FADECOLORS 351
// #define ALA_FADECOLORSLOOP 352
// #define ALA_PIXELSFADECOLORS 353

AlaSeq comet_seq[] =
    {
        {ALA_ON, 1000, 15000, alaPalCyan},
        {ALA_FADECOLORSLOOP, 6000, 40000, alaPalEaster},
        {ALA_ON, 1000, 15000, alaPalCyan},
        {ALA_PLASMA, 2000, 40000, alaPalParty},
        {ALA_ENDSEQ}};

int data;
const byte numChars = 128;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;
const uint8_t maxNumSeq = 32;

AlaPalette PaletteArray[24];
AlaSeq lightSequence[2][maxNumSeq];

uint8_t activeSequence = 0;
uint8_t inActiveSequence = 1;

void recvWithEndMarker()
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newData == false)
  {
    // size_t lineLength = Serial.readBytesUntil('\n', receivedChars, 127);
    // receivedChars[lineLength] = '\0';

    rc = Serial.read();

    if (rc != endMarker)
    {
      if ((rc != '\t') && (rc != '\r'))
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars)
        {
          ndx = numChars - 1;
        }
      }
    }
    else
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }

  if (newData)
  {
    char response[MyCommandParser::MAX_RESPONSE_SIZE];
    parser.processCommand(receivedChars, response);
    Serial.println(response);
    newData = false;
  }
}

void cmd_add(MyCommandParser::Argument *args, char *response)
{
  uint32_t idx = (uint32_t)args[0].asUInt64;
  uint32_t animation = (uint32_t)args[1].asUInt64;
  uint32_t speed = (uint32_t)args[2].asUInt64;
  uint32_t duration = (uint32_t)args[3].asUInt64;
  uint32_t palette = (uint32_t)args[4].asUInt64;

  if (palette >= 24)
  {
    palette = 1;
  }

  speed *= 1000;
  duration *= 1000;

  lightSequence[inActiveSequence][idx].animation = animation;
  lightSequence[inActiveSequence][idx].duration = duration;
  lightSequence[inActiveSequence][idx].speed = speed;
  lightSequence[inActiveSequence][idx].palette = PaletteArray[palette];

  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void cmd_swap(MyCommandParser::Argument *args, char *response)
{
  if (activeSequence == 0)
  {
    activeSequence = 1;
    inActiveSequence = 0;
  }
  else
  {
    activeSequence = 0;
    inActiveSequence = 1;
  }

  rgbStrip.setAnimation(lightSequence[activeSequence]);

  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void cmd_returnToDefault(MyCommandParser::Argument *args, char *response)
{
  for (uint8_t idx = 0; idx < maxNumSeq; idx++)
  {
    lightSequence[inActiveSequence][idx].animation = ALA_ENDSEQ;
  }

  // Copy the default Light Sequence.
  for (uint8_t idx = 0; idx < maxNumSeq; idx++)
  {
    lightSequence[inActiveSequence][idx] = comet_seq[idx];
    if (comet_seq[idx].animation == ALA_ENDSEQ)
    {
      break;
    }
  }

  if (activeSequence == 0)
  {
    activeSequence = 1;
    inActiveSequence = 0;
  }
  else
  {
    activeSequence = 0;
    inActiveSequence = 1;
  }

  rgbStrip.setAnimation(lightSequence[activeSequence]);

  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void cmd_clear(MyCommandParser::Argument *args, char *response)
{
  for (uint8_t idx = 0; idx < maxNumSeq; idx++)
  {
    lightSequence[inActiveSequence][idx].animation = ALA_ENDSEQ;
  }

  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void cmd_bright(MyCommandParser::Argument *args, char *response)
{
  uint32_t brightness = (uint32_t)args[0].asUInt64;
  float percent = (float)brightness / 100.0;

  rgbStrip.setBrightness(AlaMax(percent, percent, percent));

  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void setup()
{
  PaletteArray[0] = alaPalNull;
  PaletteArray[1] = alaPalWhite;
  PaletteArray[2] = alaPalRed;
  PaletteArray[3] = alaPalGreen;
  PaletteArray[4] = alaPalBlue;
  PaletteArray[5] = alaPalYellow;
  PaletteArray[6] = alaPalMagenta;
  PaletteArray[7] = alaPalCyan;
  PaletteArray[8] = alaPalEaster;
  PaletteArray[9] = alaPalRgb;
  PaletteArray[10] = alaPalRainbow;
  PaletteArray[11] = alaPalRainbowStripe;
  PaletteArray[12] = alaPalParty;
  PaletteArray[13] = alaPalHeat;
  PaletteArray[14] = alaPalFire;
  PaletteArray[15] = alaPalCool;
  PaletteArray[16] = alaPalNull;

  // Copy the default Light Sequence.
  for (uint8_t idx = 0; idx < maxNumSeq; idx++)
  {
    lightSequence[activeSequence][idx] = comet_seq[idx];
    if (comet_seq[idx].animation == ALA_ENDSEQ)
    {
      break;
    }
  }

  // Start Serial Stream
  Serial.begin(115200);

  leds.begin();
  leds.show();

  rgbStrip.initWS2811(&leds);

  rgbStrip.setBrightness(AlaMax(0.4, 0.4, 0.4));

  rgbStrip.setAnimation(lightSequence[activeSequence]);

  Serial.println("Heihei Rere LED Strip Control");

  if (!parser.registerCommand("SWAP", "", &cmd_swap))
  {
    Serial.println("Parser Command Add of \"SWAP\" Failed.");
  }

  if (!parser.registerCommand("DEFT", "", &cmd_returnToDefault))
  {
    Serial.println("Parser Command Add of \"DEFT\" Failed.");
  }

  if (!parser.registerCommand("CLER", "", &cmd_clear))
  {
    Serial.println("Parser Command Add of \"CLER\" Failed.");
  }

  if (!parser.registerCommand("ADDD", "uuuuu", &cmd_add))
  {
    Serial.println("Parser Command Add of \"ADDD\" Failed.");
  }

  if (!parser.registerCommand("BRIT", "u", &cmd_bright))
  {
    Serial.println("Parser Command Add of \"BRIT\" Failed.");
  }
}

void loop()
{
  rgbStrip.runAnimation();
  recvWithEndMarker();
}
