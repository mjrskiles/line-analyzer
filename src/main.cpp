//
// Created by mike on 9/20/21.
//
#include <Arduino.h>
#include <ILI9341_t3.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>


#define TFT_DC 36
#define TFT_CS 10
#define DISPLAY_X_SIZE 320
#define DISPLAY_Y_SIZE 240


// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=139,91
AudioMixer4              mixer1;         //xy=312,134
AudioOutputI2S           i2s2;           //xy=392,32
AudioAnalyzeFFT1024      fft1024;        //xy=467,147
AudioConnection          patchCord1(i2s1, 0, mixer1, 0);
AudioConnection          patchCord2(i2s1, 0, i2s2, 0);
AudioConnection          patchCord3(i2s1, 1, mixer1, 1);
AudioConnection          patchCord4(i2s1, 1, i2s2, 1);
AudioConnection          patchCord5(mixer1, fft1024);
AudioControlSGTL5000     audioShield;    //xy=366,225
// GUItool: end automatically generated code


const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;


// The scale sets how much sound is needed in each frequency range to
// show all 8 bars.  Higher numbers are more sensitive.
float scale = 1000.0;

// An array to hold the 16 frequency bands
float level[16];

// This array holds the on-screen levels.  When the signal drops quickly,
// these are used to lower the on-screen level 1 bar per update, which
// looks more pleasing to corresponds to human sound perception.
int   shown[16];

const int barPositionsX[16] = {0, 20, 40, 60,
                         80, 100, 120, 140,
                         160, 180, 200, 220,
                         240, 260, 280, 300};
const int barMaxHeight = 160;



ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

void setup() {
    AudioMemory(24);
    audioShield.enable();
    audioShield.inputSelect(myInput);
    audioShield.volume(0.5);
    audioShield.inputLevel(0.5);
    mixer1.gain(0, 0.5);
    mixer1.gain(1, 0.5);

    tft.begin();
    tft.fillScreen(ILI9341_CYAN);
    tft.setRotation(3);
}

void loop() {
    if (fft1024.available()) {
        // read the 512 FFT frequencies into 16 levels
        // music is heard in octaves, but the FFT data
        // is linear, so for the higher octaves, read
        // many FFT bins together.
        level[0] =  fft1024.read(0);
        level[1] =  fft1024.read(1);
        level[2] =  fft1024.read(2, 3);
        level[3] =  fft1024.read(4, 6);
        level[4] =  fft1024.read(7, 10);
        level[5] =  fft1024.read(11, 15);
        level[6] =  fft1024.read(16, 22);
        level[7] =  fft1024.read(23, 32);
        level[8] =  fft1024.read(33, 46);
        level[9] =  fft1024.read(47, 66);
        level[10] = fft1024.read(67, 93);
        level[11] = fft1024.read(94, 131);
        level[12] = fft1024.read(132, 184);
        level[13] = fft1024.read(185, 257);
        level[14] = fft1024.read(258, 359);
        level[15] = fft1024.read(360, 511);
        // See this conversation to change this to more or less than 16 log-scaled bands?
        // https://forum.pjrc.com/threads/32677-Is-there-a-logarithmic-function-for-FFT-bin-selection-for-any-given-of-bands

//        tft.fillScreen(ILI9341_BLUE);

        for (int i=0; i<16; i++) {
            tft.setCursor(barPositionsX[i], 230);
            tft.print(i);
            Serial.print(level[i]);

            // TODO: conversion from FFT data to display bars should be
            // exponentially scaled.  But how keep it a simple example?
            int val = level[i] * scale;
            if (val > barMaxHeight) val = barMaxHeight;

            if (val >= shown[i]) {
                shown[i] = val;
            } else {
                if (shown[i] > 0) shown[i] = shown[i] - (scale / 300.0f);
                val = shown[i];
            }

            //Serial.print(shown[i]);
            Serial.print(" ");



            int rectY = DISPLAY_Y_SIZE - val;
            tft.fillRect(barPositionsX[i], rectY, 19, val, ILI9341_MAGENTA);
            tft.fillRect(barPositionsX[i], 0, 19, rectY - 1, ILI9341_CYAN);
        }
        Serial.print(" cpu:");
        Serial.println(AudioProcessorUsageMax());
    }
}

/*
 *
0    0
   1    1
   2    2
   3    3
   4    4
   5    5
   6    6
   7    7
   8    8
   9    9
  10   10
  11   11
  12   12
  13   13
  14   14
  15   16
  17   18
  19   20
  21   22
  23   24
  25   26
  27   28
  29   30
  31   32
  33   34
  35   36
  37   38
  39   40
  41   42
  43   44
  45   46
  47   48
  49   51
  52   54
  55   57
  58   60
  61   63
  64   66
  67   69
  70   72
  73   75
  76   78
  79   81
  82   84
  85   88
  89   92
  93   96
  97  100
 101  104
 105  108
 109  112
 113  116
 117  120
 121  125
 126  130
 131  135
 136  140
 141  145
 146  150
 151  156
 157  162
 163  168
 169  174
 175  180
 181  186
 187  193
 194  200
 201  207
 208  214
 215  221
 222  229
 230  237
 238  245
 246  253
 254  261
 262  270
 271  279
 280  288
 289  298
 299  308
 309  318
 319  328
 329  339
 340  350
 351  361
 362  373
 374  385
 386  397
 398  410
 411  423
 424  437
 438  451
 452  465
 466  480
 481  495
 496  511

 */