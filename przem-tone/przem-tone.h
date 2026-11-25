/* przem-low-shelf - filtr półkowy dolnoprzepustowy
 * 26-11-2026
 * © Przemysław R. Pietraszczyk
 * licencja: MIT
 */
#include <ladspa.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PORTCOUNT   6

/* The Unique ID of the plugin: */

#define ID_STEREO         1129959

/* The port numbers for the plugin: */

#define DIRECTION       0
#define OCTAVE          1
#define INPUTL          2
#define INPUTR          3
#define OUTPUTL         4
#define OUTPUTR         5

#define BUF_SIZE    2000
//#define LIMIT(v,l,u) ((v)<(l)?(l):((v)>(u)?(u):(v)))


typedef struct {
    LADSPA_Data *direction;   // Port kontrolny: bass czy wysokie
    LADSPA_Data *octave;      // Port kontrolny: oktawa
    LADSPA_Data *inputl;     // Port wejściowy audio
    LADSPA_Data *inputr;     // Port wejściowy audio
    LADSPA_Data *outputl;    // Port wyjściowy audio
    LADSPA_Data *outputr;    // Port wyjściowy audio

    double sample_rate;     // Częstotliwość próbkowania (z funkcji instantiate)
} TonePlugin;
