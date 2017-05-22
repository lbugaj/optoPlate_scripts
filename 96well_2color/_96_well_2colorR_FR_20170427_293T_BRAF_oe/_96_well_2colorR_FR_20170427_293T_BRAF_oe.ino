/*

In this script, compared to others, the delay and OFF cycles are at defined red/FR ratios. Goal is to boost basal Erk levels to match SB levels.  
For best results in test mode (while eliinating pulses), comment out the switchTo variables during phase changes.  But then remember to uncomment when leaving test mode!

*/

#include "Adafruit_TLC5947.h"
#include "Arduino.h"
#include <avr/pgmspace.h>



// How many boards do you have chained?
const uint8_t NUM_TLC5974 = 12;

const uint8_t data  = 4;
const uint8_t clock = 5;
const uint8_t latch = 6;
const uint8_t oe  = 7;  // set to -1 to not use the enable pin (its optional)

//defines numbers of channels total for all colors (288 channels for 3 col 96 well plate)
const int chanNum = 24*NUM_TLC5974;

//defines state of pins (0 = OFF)
uint8_t ledState[chanNum/3] = {0};

//Cycle number (for fine pulsing)
uint16_t cycNum[chanNum/3] = {0};

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);

// Define whether individual positions will be OFF (0), in timecourse mode (1), or in constant light setting mode (2) (in flash memory, read only)  Rows: A-H. Columns: 1-12                                  
const uint8_t LEDmode[] PROGMEM ={ 
  // 1   2   3   4   5   6   7   8   9  10  11  12


/*
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	1,	1,	3,	3,	0,	0,	1,	1,	3,	3,	1,
0,	1,	1,	3,	3,	0,	0,	1,	1,	3,	3,	1,
0,	1,	1,	3,	3,	0,	0,	1,	1,	3,	3,	1,
0,	1,	1,	3,	3,	0,	0,	1,	1,	3,	3,	1,
0,	1,	1,	3,	3,	0,	0,	1,	1,	3,	3,	1,
0,	1,	1,	3,	3,	0,	0,	1,	1,	3,	3,	1,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0


};
*/
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
 }; 
 
 
/*
 0,0,0,0,0,0,0,0,0,0,0,0,
 1,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0};
 */



const uint8_t OFF = 0;
const uint8_t TIMECOURSE = 1;
const uint8_t CONST_INTENSITY = 2;


//Pulse intervals (flash memory, read only)  Rows: A-H. Columns: 1-12
const uint32_t interval[] PROGMEM = {
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,
5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,	5000,

};

//Pulse widths (flash memory, read only). Rows: A-H. Columns: 1-12
const uint32_t pulse[] PROGMEM = { 
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,
500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,	500,

};

//LED ON intensity, for variable intensity setting (flash memory, read only). Rows: A-H. Columns: 1-12
const uint16_t intensityOn[] PROGMEM = { 
//RED Intensities
25,  2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 1000,
50,  2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 750,
100, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 500,
250, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 250,
500, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 100,
750, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 50,
1000,  2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 25,
0,  0,  0,  0,  0,  0,  2000, 2000, 0,  0,  0,  0,

  //FAR-RED INTENTSITIES
2000,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2000,
2000, 0,  0,  0,  2000,  2000,  0,  0,  0,  0,  0,  2000,
};



//LED OFF intensity, for variable intensity setting (flash memory, read only). Rows: A-H. Columns: 1-12

const uint16_t intensityOff[] PROGMEM = { 
 //RED Intensities
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,
0,  0,  50, 100,  250,  500,  1000, 500,  350,  100,  50, 0,

  //FAR-RED INTENTSITIES  
2000,  2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
};


//number of phases in timecourse (including initial delay phase
const uint8_t totPhaseNum = 3;

//Set intensities for various phases. {Red,Far-Red}
uint16_t phaseOFF[2] = {0,0};
uint16_t phaseDELAY[2] = {0,4095};
uint16_t phase1int[2] = {4095,0};
uint16_t phase2int[2] = {0,4095};

//array of phases
uint16_t *phases[totPhaseNum+1] = {phaseOFF, phaseDELAY, phase1int, phase2int};

//define the current phase (0-3), with 0 being OFF
uint8_t currPhase[chanNum/3] = {0};

//Time variables for pulse lengths
uint32_t s0 = 0;
uint32_t s1 = 1000UL*1UL;
uint32_t s5 = 1000UL*5UL;
uint32_t s10 = 1000UL*10UL;
uint32_t s6 = 1000UL*6UL;
uint32_t s12 = 1000UL*12UL;
uint32_t s18 = 1000UL*18UL;
uint32_t s20 = 1000UL*20UL;
uint32_t s24 = 1000UL*24UL;
uint32_t s30 = 1000UL*30UL;
uint32_t s36 = 1000UL*36UL;
uint32_t s42 = 1000UL*42UL;
uint32_t s45 = 1000UL*45UL;
uint32_t s48 = 1000UL*48UL;
uint32_t s60 = 1000UL*60UL;
uint32_t s120 = 1000UL*120UL;
uint32_t s150 = 1000UL*150UL;
uint32_t s170 = 1000UL*170UL;

uint32_t  m3 = 60UL*1000UL*3UL;
uint32_t  m4 = 60UL*1000UL*4UL;
uint32_t  m5 =  60UL*1000UL*5UL;
uint32_t  m6 =  60UL*1000UL*6UL;
uint32_t  m7 =  60UL*1000UL*7UL;
uint32_t  m8 =  60UL*1000UL*8UL;
uint32_t  m9 =  60UL*1000UL*9UL;
uint32_t  m9_15 =  60UL*1000UL*9.25;
uint32_t  m9_30 =  60UL*1000UL*9.5;
uint32_t  m9_40 =  60UL*1000UL*9.66;
uint32_t  m9_50 =  60UL*1000UL*9.83;
uint32_t  m9_55 =  60UL*1000UL*9.916;
uint32_t  m10 = 60UL*1000UL*10UL;
uint32_t  m15 = 60UL*1000UL*15UL;
uint32_t  m20 = 60UL*1000UL*20UL;
uint32_t  m25 = 60UL*1000UL*25UL;
uint32_t  m30 = 60UL*1000UL*30UL;
uint32_t  m35 = 60UL*1000UL*35UL;
uint32_t  m40 = 60UL*1000UL*40UL;
uint32_t  m45 = 60UL*1000UL*45UL;
uint32_t  m50 = 60UL*1000UL*50UL;
uint32_t  m51 = 60UL*1000UL*51UL;
uint32_t  m52 = 60UL*1000UL*52UL;
uint32_t  m53 = 60UL*1000UL*53UL;
uint32_t  m54 = 60UL*1000UL*54UL;
uint32_t  m55 = 60UL*1000UL*55UL;
uint32_t  m56 = 60UL*1000UL*56UL;
uint32_t  m58 = 60UL*1000UL*58UL;
uint32_t  m60 = 60UL*1000UL*60UL;
uint32_t  m65 = 60UL*1000UL*65UL;
uint32_t  m70 = 60UL*1000UL*70UL;
uint32_t  m75 = 60UL*1000UL*75UL;
uint32_t  m80 = 60UL*1000UL*80UL;
uint32_t  m85 = 60UL*1000UL*85UL;
uint32_t  m88 = 60UL*1000UL*88UL;
uint32_t  m89 = 60UL*1000UL*89UL;
uint32_t  m89_30 = 60UL*1000UL*89.5;
uint32_t m90 = 60UL*1000UL*90UL;
uint32_t m95 = 60UL*1000UL*95UL;
uint32_t m105 = 60UL*1000UL*105UL;
uint32_t m110 = 60UL*1000UL*110UL;
uint32_t m115 = 60UL*1000UL*115UL;
uint32_t m120 = 60UL*1000UL*120UL;
uint32_t m125 = 60UL*1000UL*125UL;
uint32_t m130 = 60UL*1000UL*130UL;
uint32_t m135 = 60UL*1000UL*135UL;
uint32_t m140 = 60UL*1000UL*140UL;
uint32_t m150 = 60UL*1000UL*150UL;
uint32_t m165 = 60UL*1000UL*165UL;
uint32_t m170 = 60UL*1000UL*170UL;
uint32_t m180 = 60UL*1000UL*180UL;
uint32_t m190 = 60UL*1000UL*190UL;
uint32_t m195 = 60UL*1000UL*195UL;
uint32_t m210 = 60UL*1000UL*210UL;
uint32_t m220 = 60UL*1000UL*220UL;
uint32_t m230 = 60UL*1000UL*230UL;
uint32_t m225 = 60UL*1000UL*225UL;
uint32_t m240 = 60UL*1000UL*240UL;
uint32_t m250 = 60UL*1000UL*250UL;
uint32_t m255 = 60UL*1000UL*255UL;
uint32_t m260 = 60UL*1000UL*260UL;
uint32_t m265 = 60UL*1000UL*265UL;
uint32_t m270 = 60UL*1000UL*270UL;
uint32_t m285 = 60UL*1000UL*285UL;
uint32_t m300 = 60UL*1000UL*300UL;

uint32_t h1  = 1000UL*60UL*60UL*1UL;
uint32_t h2  = 1000UL*60UL*60UL*2UL;
uint32_t h3  = 1000UL*60UL*60UL*3UL;
uint32_t h4  = 1000UL*60UL*60UL*4UL;
uint32_t h6  = 1000UL*60UL*60UL*6UL;
uint32_t h8  = 1000UL*60UL*60UL*8UL;
uint32_t h10  = 1000UL*60UL*60UL*10UL;
uint32_t h12 = 1000UL*60UL*60UL*12UL;
uint32_t h14 = 1000UL*60UL*60UL*14UL;
uint32_t h16 = 1000UL*60UL*60UL*16UL;
uint32_t h18 = 1000UL*60UL*60UL*18UL;
uint32_t h20 = 1000UL*60UL*60UL*20UL;
uint32_t h22 = 1000UL*60UL*60UL*22UL;
uint32_t h24 = 1000UL*60UL*60UL*24UL;
uint32_t h26 = 1000UL*60UL*60UL*26UL;
uint32_t h30 = 1000UL*60UL*60UL*30UL;
uint32_t h36 = 1000UL*60UL*60UL*36UL;
uint32_t h42 = 1000UL*60UL*60UL*42UL;
uint32_t h48 = 1000UL*60UL*60UL*48UL;


//defines timecourse for each channel as array of pointers
const uint32_t * const phaseTimes[chanNum/3][totPhaseNum] PROGMEM = {

  //1                         2                    3                        4                      5                     6                        7                      8                      9                      10                        11                    12 
{&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},  {&h24,&m5,&m10},
{&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10}, {&m60,&h24,&m10},
{&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24}, {&m58,&m60,&h24},
{&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24}, {&m55,&m60,&h24},
{&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24}, {&m50,&m60,&h24},
{&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24}, {&m40,&m60,&h24},
{&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24}, {&m20,&m60,&h24},
{&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},  {&s0,&m60,&h24},
};
//Define constant off times for OFF_TIMECOURSE setting
const uint32_t * const off_millis[] PROGMEM ={ 
// 1     2     3       4       5       6       7       8       9      10      11      12
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&m5,
&m10,	&m10,	&m10,	&m10,	&m10,	&m10,	&m10,	&m10,	&m5,	&m5,	&m5,	&m5,
&m10,	&m10,	&m10,	&m10,	&m10,	&m10,	&m10,	&m10,	&m5,	&m5,	&m5,	&m5,

}; //H   


//Define constant on times for variable constant Red ON times
const uint32_t * const on_millis[] PROGMEM ={ 
                               // 1   2   3   4   5   6   7   8   9  10  11  12
&s0,	&s0,	&s0,	&s0,	&s0,	&s0,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&s0,	&s0,	&s0,	&s0,	&s0,	&s0,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&s10,	&s10,	&s10,	&s10,	&s10,	&s10,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&s30,	&s30,	&s30,	&s30,	&s30,	&s30,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&s60,	&s60,	&s60,	&s60,	&s60,	&s60,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&m5,	&m5,	&m5,	&m5,	&m5,	&m5,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,	&s120,
};  

// define time that IR or Red will be constantly on after ON->OFF transition and OFF->ON transition
uint32_t *IRoffSwitch =  &m6;
uint32_t *REDonSwitch =  &s60;

//define pointers to be used in script for pointing to either a constant switch time or to the variable switch times (on_millis, offmillis)
uint32_t *offTime;
uint32_t *onTime;

//define flag to determine whther a channel has just finished an ON phase
uint8_t switchTo[chanNum/3] = { 
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0};

//define if using phase intensities or variable intensities. 0 to use phase intensities, 1 to use variable.  Can do this for ON intensities, OFF intensities
uint8_t useVarOnIntensity = 0;
uint8_t useVarOffIntensity = 0;


//define if pulsed or constant. 1 for pulsed, 0 for constant
uint8_t pulsed = 1;

//define if in test mode. If in test mode, define "factor" for how much to divide phase times by
uint8_t test = 0;

uint32_t factor = 60UL; //times change from hours to seconds

//keep track of when switched to the current phase
uint32_t lastPhaseSwitch[chanNum/3] = {0};

//use variable times for ON or OFF constant illumination when to red or IR (1 yes, 0 no)?
uint8_t varON = 0;
uint8_t varOFF = 0;

uint8_t flag = 0;



void setup() {
  Serial.begin(9600);
  tlc.begin();

  //set initial LED states to 0
  for (int i = 0; i < chanNum/3; i++){
    
    setAll(i,phaseOFF);
    cycNum[i] = 0;
  }
  tlc.write();

  if (oe >= 0) {
    pinMode(oe, OUTPUT);
    digitalWrite(oe, HIGH);
  }

  //if in test mode, divide phase times by defined factor amount to visualize entire timecourse
  if (test){
    s1 = (uint32_t)s1/factor;
    s5 = (uint32_t)s5/factor;
    s10 = (uint32_t)s10/factor;
    s30 = (uint32_t)s30/factor;
    s60 = (uint32_t)s60/factor;
    s120 = (uint32_t)s120/factor;

    m3 =  (uint32_t)m3/factor; 
    m4 =  (uint32_t)m4/factor;
    m5 =  (uint32_t)m5/factor;
    m6 =  (uint32_t)m6/factor;
    m8 =  (uint32_t)m8/factor;
    m10 = (uint32_t)m10/factor;
    m15 = (uint32_t)m15/factor;
    m20 = (uint32_t)m20/factor;
    m25 = (uint32_t)m25/factor;
    m30 = (uint32_t)m30/factor;
    m35 = (uint32_t)m35/factor;
    m40 = (uint32_t)m40/factor;
    m45 = (uint32_t)m45/factor;
    m50 = (uint32_t)m50/factor;
    m51 = (uint32_t)m51/factor;
    m52 = (uint32_t)m52/factor;
    m53 = (uint32_t)m53/factor;
    m54 = (uint32_t)m54/factor;
    m55 = (uint32_t)m55/factor;
    m56 = (uint32_t)m56/factor;
    m58 = (uint32_t)m58/factor;
    m60 = (uint32_t)m60/factor;
     m65 = (uint32_t)m65/factor;
    m70 = (uint32_t)m70/factor;
    m75 = (uint32_t)m75/factor;
    m80 = (uint32_t)m80/factor;
    m85 = (uint32_t)m85/factor;
    m90 = (uint32_t)m90/factor;
    m95 = (uint32_t)m95/factor;
    m105 = (uint32_t)m105/factor;
    m110 = (uint32_t)m110/factor;
    m115 = (uint32_t)m115/factor;
    m120 =(uint32_t)m120/factor;
    m125 =(uint32_t)m125/factor;
    m130 =(uint32_t)m130/factor;
    m135 =(uint32_t)m135/factor;
    m140 =(uint32_t)m140/factor;
    m150 =(uint32_t)m150/factor;
    m165 =(uint32_t)m165/factor;
    m170 =(uint32_t)m170/factor;
    m180 =(uint32_t)m180/factor;
    m190 =(uint32_t)m190/factor;
    m195 =(uint32_t)m195/factor;
    m210 =(uint32_t)m210/factor;
    m220 =(uint32_t)m220/factor;
    m225 =(uint32_t)m225/factor;
    m230 =(uint32_t)m230/factor;
    m240 =(uint32_t)m240/factor;
    m250 =(uint32_t)m250/factor;
    m255 =(uint32_t)m255/factor;
    m260 =(uint32_t)m260/factor;
    m265 =(uint32_t)m265/factor;
    m270 =(uint32_t)m270/factor;
    m285 =(uint32_t)m285/factor;
    m300 =(uint32_t)m300/factor;
    
    h1 =  (uint32_t)h1/factor;
    h2 =  (uint32_t)h2/factor;
    h3 =  (uint32_t)h3/factor; 
    h4 =  (uint32_t)h4/factor;
    h6 =  (uint32_t)h6/factor;
    h8 =  (uint32_t)h8/factor;
    h10 =  (uint32_t)h10/factor;
    h12 = (uint32_t)h12/factor;
    h14 =  (uint32_t)h14/factor;
    h16 = (uint32_t)h16/factor;
    h18 = (uint32_t)h18/factor;
    h20 = (uint32_t)h20/factor;
    h22 = (uint32_t)h22/factor;
    h24 = (uint32_t)h24/factor;
    h26 = (uint32_t)h26/factor;
    h30 = (uint32_t)h30/factor;
    h36 = (uint32_t)h36/factor;
    h42 = (uint32_t)h42/factor;
    h48 = (uint32_t)h48/factor;
  }

  //set initial LED states to first pulse intensity
  for (int i = 0; i < chanNum/3; i++){

    uint8_t mode = pgm_read_word_near(LEDmode+i);

    if(mode == TIMECOURSE){
      if (useVarOffIntensity){
        uint16_t redInt = pgm_read_word_near(intensityOff+i);
        uint16_t fredInt = pgm_read_word_near(intensityOff+i+96);
        uint16_t intensities[2] = {redInt, fredInt};
        setAll(i, intensities);
        currPhase[i] = 1;
        ledState[i] = 1;
      }
      else{
        setAll(i,phaseDELAY);
        currPhase[i] = 1;
        ledState[i] = 1;
      }
    }
  }
  tlc.write();
}


void loop(){

  uint8_t changed = 0;
  unsigned long currMillis = millis();
  

  //Identify correct illlumination phase, update LED settings
  for (uint16_t i = 0; i<chanNum/3; i++){
    uint8_t mode = pgm_read_word_near(LEDmode+i);
    uint8_t phaseChanged = 0;

    if(mode != OFF){

      if(mode == TIMECOURSE){       //Loops to compare current time against timecourse timing, and also to hold light settings at last timepoint 
        uint32_t *phaseT = (uint32_t*)pgm_read_word_near(&phaseTimes[i][currPhase[i]-1]);
        // Serial.println(*phaseT);

        if (currMillis -lastPhaseSwitch[i] > *phaseT){
          lastPhaseSwitch[i] = lastPhaseSwitch[i] + *phaseT;
          phaseChanged = 1;
          if (currPhase[i] == 2){  //if phase is 2, go to 3
            currPhase[i] = 3;
            if(!test){
              switchTo[i] = 1; //Set flag to indicate that channel has just switched from ON to OFF
            }
          } 
          else{
             currPhase[i] = 2;      //if phase is 1(delay) or 3, go to 2
             if(!test){
              switchTo[i] = 2; //Set flag to indicate that channel has just switched from OFF to ON
             }
          }       
        }
      }

      //Setting new LED settings in constant (non-pulsed) light setting
      if (!pulsed){
        if(mode == TIMECOURSE){
          if (phaseChanged){
            if (currPhase[i] == 2 && useVarOnIntensity){
              uint16_t redInt = pgm_read_word_near(intensityOn+i);
              uint16_t fredInt = pgm_read_word_near(intensityOn+i+96);
              uint16_t intensities[2] = {redInt, fredInt};
              setAll(i, intensities);
              changed = 1;
            }
            else if (currPhase[i] == 3 && useVarOffIntensity){
              uint16_t redInt = pgm_read_word_near(intensityOff+i);
              uint16_t fredInt = pgm_read_word_near(intensityOff+i+96);
              uint16_t intensities[2] = {redInt, fredInt};
              setAll(i, intensities);
              changed = 1;
            }
            else{
              setAll(i,phases[currPhase[i]]);
              changed = 1;
            }
          }  
        } 
        else if (mode == CONST_INTENSITY){

           uint16_t redInt = pgm_read_word_near(intensityOn+i);
           uint16_t fredInt = pgm_read_word_near(intensityOn+i+96);
           uint16_t intensities[2] = {redInt, fredInt};
           setAll(i, intensities);
           changed = 1;

        }
      }      

      //Setting new LED settings in pulsed light setting
      if (pulsed){
        uint32_t memInterval = pgm_read_dword(interval+i);
        uint32_t memPulse = pgm_read_dword(pulse+i);
        uint32_t prevMillis = (uint32_t)cycNum[i]*memInterval;

        if (currMillis-prevMillis > memInterval && ledState[i] == 0) { //if interval time expired, turn LED ON
          ledState[i] = 1;          //set LED flag to ON
          cycNum[i] = cycNum[i]+1;  //increment cycle
          if (mode == TIMECOURSE){ 
            if (currPhase[i] == 2 && useVarOnIntensity){
              uint16_t redInt = pgm_read_word_near(intensityOn+i);
              uint16_t fredInt = pgm_read_word_near(intensityOn+i+96);
              uint16_t intensities[2] = {redInt, fredInt};
              setAll(i, intensities);
              changed = 1;
            }
            else if (currPhase[i] == 3 && useVarOffIntensity){
              uint16_t redInt = pgm_read_word_near(intensityOff+i);
              uint16_t fredInt = pgm_read_word_near(intensityOff+i+96);
              uint16_t intensities[2] = {redInt, fredInt};
              setAll(i, intensities);
              changed = 1;
            }
            else if (currPhase[i] == 1 && useVarOffIntensity){
              uint16_t redInt = pgm_read_word_near(intensityOff+i);
              uint16_t fredInt = pgm_read_word_near(intensityOff+i+96);
              uint16_t intensities[2] = {redInt, fredInt};
              setAll(i, intensities);
              changed = 1;
            }
            else{
              setAll(i, phases[currPhase[i]]); //set all LED intensities to the current phase
              changed = 1;        //set the "LED changed" flag to write to LEDs
            }
          }
          else if (mode == CONST_INTENSITY){  
             uint16_t redInt = pgm_read_word_near(intensityOn+i);
             uint16_t fredInt = pgm_read_word_near(intensityOn+i+96);
             uint16_t intensities[2] = {redInt, fredInt};
             setAll(i, intensities);
             changed = 1;
          }
        }
        else {
          if (currMillis-prevMillis> memPulse && ledState[i] == 1){  // if pulse time expired, turn LED OFF
            ledState[i] = 0;             //set LED flag to OFF
            changed = 1;                 //set the "LED changed" flag to write to LEDs
            setAll(i,phaseOFF);         //set LED OFF   
          }  
        } 
      }

      //Loops to check if LED as just turned from ON to OFF. If so, keep IR ON constantly.
      
      
 //Define constant OFF time based on setup
 if (switchTo[i] == 1){
   if (varOFF){
    offTime =  (uint32_t*)pgm_read_word_near(off_millis+i);}
   else {
    offTime =  IRoffSwitch;}
  }
// Define constant ON time based on setup
 if (switchTo[i] == 2){
   flag = 5;
   if (varON){
    onTime =  (uint32_t*)pgm_read_word_near(on_millis+i);}
   else {
    onTime =  REDonSwitch;}
 }

      
      
      
      
if (mode == TIMECOURSE){
  if (switchTo[i] ==1 && (currMillis-lastPhaseSwitch[i]) < *IRoffSwitch){
    if (useVarOffIntensity){
      uint16_t redInt = pgm_read_word_near(intensityOff+i);
       uint16_t fredInt = pgm_read_word_near(intensityOff+i+96);
       uint16_t intensities[2] = {redInt, fredInt};
       setAll(i, intensities);
       changed = 1;}
    else {
      setAll(i,phase2int);
      changed = 1;}
      }
  else if (switchTo[i] == 1 && (currMillis - lastPhaseSwitch[i]) > *IRoffSwitch){
    switchTo[i] = 0;
    setAll(i,phaseOFF);
    changed = 1;}
  else if (switchTo[i] == 2 && (currMillis - lastPhaseSwitch[i]) < *onTime){
     if (useVarOnIntensity){
       uint16_t redInt = pgm_read_word_near(intensityOn+i);
       uint16_t fredInt = pgm_read_word_near(intensityOn+i+96);
       uint16_t intensities[2] = {redInt, fredInt};
       setAll(i, intensities);
       changed = 1;}
     else{
       setAll(i,phase1int);
       changed = 1;}
    }
  else if (switchTo[i] == 2 && (currMillis - lastPhaseSwitch[i]) > *onTime){
    switchTo[i] = 0;
    setAll(i,phaseOFF);
    ledState[i] = 0;
    changed = 1;}
}
 }
}

  //if LED settings were updated, then write new settings to LEDs
  if (changed){
    /*
    Serial.print("Current phase: ");
     Serial.println(currPhase[91]);
     Serial.print("Current time: ");
     Serial.println(currMillis);
     Serial.print("switchTo: ");
     Serial.println(switchToswitchTo[91]);
     Serial.print("CurrMils - lastphaseswitch: ");
     Serial.println(currMillis - lastPhaseSwitch[91]);
     Serial.print("IRoffSwitch");
     Serial.println(*IRoffSwitch);
     Serial.print("Flag: ");
     Serial.println(flag);
     */
    tlc.write();
  }
}



//FUNCTIONS to control LED colors
void setRed(uint16_t well, uint16_t *bright){
  uint16_t redPosition = (uint16_t)(well+192);
  tlc.setPWM(redPosition, *(bright));   //Set Far-red
}

void setFarRed1(uint16_t well, uint16_t *bright){
  uint16_t farRed1Position = (uint16_t)((int)(well/12) + 8*(well%12));
  tlc.setPWM(farRed1Position, *(bright+1));   //Set Blue
}

void setFarRed2(uint16_t well, uint16_t *bright){
  uint16_t farRed2Position = (uint16_t)((int)(well/12) + 8*(well%12)+96);
  tlc.setPWM(farRed2Position, *(bright+1));   //Set Red
}



void setAll(uint16_t well, uint16_t *bright){
  setFarRed1(well, bright);
  setFarRed2(well, bright);
  setRed(well, bright);
}


