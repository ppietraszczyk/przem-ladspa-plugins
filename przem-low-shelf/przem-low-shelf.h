/* przem-low-shelf - filtr półkowy dolnoprzepustowy
 * 17-11-2026
 * © Przemysław R. Pietraszczyk
 * licencja: MIT
 */
#include <ladspa.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#define PORTCOUNT_MONO   6

/* The Unique ID of the plugin: */

#define ID_MONO         1129958

/* The port numbers for the plugin: */

#define GAIN       0
#define FREQ            1
#define INPUTL          2
#define INPUTR          3
#define OUTPUTL         4
#define OUTPUTR         5

#define BUF_SIZE    2000
//#define LIMIT(v,l,u) ((v)<(l)?(l):((v)>(u)?(u):(v)))


typedef struct {
    LADSPA_Data *gain;   // Port kontrolny: podbicie w dB (np. 0 do +12 dB)
    LADSPA_Data *freq;      // Port kontrolny: częstotliwość graniczna (np. 100 Hz)
    LADSPA_Data *inputl;     // Port wejściowy audio
    LADSPA_Data *inputr;     // Port wejściowy audio
    LADSPA_Data *outputl;    // Port wyjściowy audio
    LADSPA_Data *outputr;    // Port wyjściowy audio

    double sample_rate;     // Częstotliwość próbkowania (z funkcji instantiate)

    // Pamięć stanu filtra (poprzednie próbki)
    double Lx1, Lx2;
    double Ly1, Ly2;
    double Rx1, Rx2;
    double Ry1, Ry2;
} BassBoostPlugin;
