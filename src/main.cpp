#include <OctoWS2811.h>
#include <AlaLedRgb.h>
#include <CommandParser.h>

typedef CommandParser<> MyCommandParser;

MyCommandParser parser;

const int ledsPerStrip = 150;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

uint8_t HeiheiPinList[4] = {2, 14, 6, 20};

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, 4, HeiheiPinList);
AlaLedRgb rgbStrip;
// animation, speed, duration, AlaPalette
AlaSeq default_seq[] =
{
  { ALA_OFF,            2000, 4000, alaPalNull },
  { ALA_ON,             2000, 4000, alaPalEaster },
  { ALA_SPARKLE,        2000, 18000, alaPalEaster },
  { ALA_CYCLECOLORS,    6000, 12000, alaPalEaster },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_FADECOLORSLOOP, 6000, 12000, alaPalEaster },
  { ALA_SPARKLE2,       2000, 12000, alaPalEaster },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_SPARKLE2,       2000, 12000, alaPalFire },
  { ALA_PIXELSMOOTHSHIFTRIGHT, 16000, 5000, alaPalEaster },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_MOVINGBARS,     6000, 32000, alaPalEaster },
  { ALA_COMET,          6000, 12000, alaPalEaster },
  { ALA_COMETCOL,       6000, 12000, alaPalEaster },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_GLOW,           6000, 12000, alaPalEaster },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_FIRE,           2000, 12000, alaPalFire },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_BOUNCINGBALLS,  2000, 12000, alaPalEaster },
  { ALA_OFF,            1000, 1000, alaPalNull },
  { ALA_BUBBLES,        2000, 12000, alaPalRainbow },
  { ALA_ENDSEQ }
};

AlaSeq comet_seq[] =
{
  { ALA_OFF,            1000, 1000,  alaPalNull },
  { ALA_COMET,          6000, 18000, alaPalRgb  },
  { ALA_ENDSEQ }
};

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
    //size_t lineLength = Serial.readBytesUntil('\n', receivedChars, 127);
    //receivedChars[lineLength] = '\0';

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

  if(newData)
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

  if(palette > 7)
  {
    palette = 1;
  }

  lightSequence[inActiveSequence][idx].animation = animation;
  lightSequence[inActiveSequence][idx].duration = duration;
  lightSequence[inActiveSequence][idx].speed = speed;
  lightSequence[inActiveSequence][idx].palette = PaletteArray[palette];

  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void cmd_swap(MyCommandParser::Argument *args, char *response)
{
  if(activeSequence == 0)
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
  for(uint8_t idx = 0; idx < maxNumSeq; idx++)
  {
    lightSequence[inActiveSequence][idx].animation = ALA_ENDSEQ;
  }

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

  //Copy the default Light Sequence.
  for(uint8_t idx = 0; idx < maxNumSeq; idx++)
  {
    lightSequence[activeSequence][idx] = comet_seq[idx];
    if(default_seq[idx].animation == ALA_ENDSEQ)
    {
      break;
    }
  }

  // Start Serial Stream
  Serial.begin(115200);

  parser.registerCommand("SWAP", "", &cmd_swap);
  parser.registerCommand("CLER", "", &cmd_clear);
  parser.registerCommand("ADDD", "uuuuu", &cmd_add);

  leds.begin();
  leds.show();

  rgbStrip.initWS2811(&leds);
  
  rgbStrip.setBrightness(AlaMax(0.75, 0.75, 0.75));

  rgbStrip.setAnimation(lightSequence[activeSequence]);
}

void loop() 
{
  rgbStrip.runAnimation();
  recvWithEndMarker();
}
