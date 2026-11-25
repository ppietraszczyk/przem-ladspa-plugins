/* przem-low-shelf - filtr półkowy dolnoprzepustowy
 * 26-11-2026
 * © Przemysław R. Pietraszczyk
 * licencja: MIT
 */
#include "przem-tone.h"

static LADSPA_Handle instantiate(const LADSPA_Descriptor *descriptor,
                                 unsigned long sample_rate)
{
    LADSPA_Handle plugin;

    if ((plugin = calloc(1, sizeof(TonePlugin))) != NULL) {
        ((TonePlugin *)plugin)->sample_rate = sample_rate;
        return plugin;
    }
    return NULL;
}

static void connect_port(LADSPA_Handle handle, unsigned long port,
                         LADSPA_Data *value)
{
    TonePlugin *plugin = (TonePlugin *) handle;

    switch (port) {
        case DIRECTION:
            plugin->direction = value;
            break;
        case OCTAVE:
            plugin->octave = value;
            break;
        case INPUTL:
            plugin->inputl = value;
            break;
        case INPUTR:
            plugin->inputr = value;
            break;
        case OUTPUTL:
            plugin->outputl = value;
            break;
        case OUTPUTR:
            plugin->outputr = value;
            break;
    }
}

void activate(LADSPA_Handle handle)
{
    //TonePlugin *plugin = (TonePlugin *) handle;
}

static void run(LADSPA_Handle handle, unsigned long SampleCount)
{
    TonePlugin *plugin = (TonePlugin *) handle;

    LADSPA_Data *inputl = plugin->inputl;
    LADSPA_Data *inputr = plugin->inputr;
    LADSPA_Data *outputl = plugin->outputl;
    LADSPA_Data *outputr = plugin->outputr;

    // Pobranie wartości z portów
    double direction = *(plugin->direction);
    double octave = *(plugin->octave);

    if (octave < 0.01) return;

    int k = (int)(octave * 10);

    double fs = plugin->sample_rate;
    if (fs <= 0.0) return; // Zabezpieczenie przed błędnym sample_rate

    int z, w;
    unsigned int s;

    // obniża o oktawe dla 0.2
    if (direction == 0) {
        system("echo '-NISKIE-' >> /tmp/PRZEMTONE.txt");
        for (z = 0, s = 0; s <  SampleCount - k; s += k, z += 1) {
            w = 0;
            do {
                *(outputl+s+w) = *(inputl+z);
                *(outputr+s+w) = *(inputr+z);
            }
            while (++w < k);
        }
    }
    else {
        system("echo '-WYSOKIE-' >> /tmp/PRZEMTONE.txt");
        unsigned int g = 0;
        for (z = 0, s = k; z <  SampleCount - k; s++, z += 1) {
            w = 0;
            do {
                *(outputl+s) = *(inputl+g);
                *(outputr+s) = *(inputr+g);
                g += w;
            } while (++w < k);
        }
    }
}

static void cleanup(LADSPA_Handle handle)
{
    free(handle);
}

LADSPA_Descriptor * stereo_descriptor = NULL;

void
__attribute__((constructor)) sin_init() {

    char ** port_names;
    LADSPA_PortDescriptor * port_descriptors;
    LADSPA_PortRangeHint * port_range_hints;

    if ((stereo_descriptor =
        (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor))) == NULL)
        exit(1);

    stereo_descriptor->UniqueID = ID_STEREO;
    stereo_descriptor->Label = strdup("Przem Tone");
    stereo_descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE; //0;
    stereo_descriptor->Name = strdup("Przem Tone");
    stereo_descriptor->Maker = strdup("Przemek R. Pietraszczyk");
    stereo_descriptor->Copyright = strdup("MIT");
    stereo_descriptor->PortCount = PORTCOUNT;

    if ((port_descriptors =
        (LADSPA_PortDescriptor *)calloc(PORTCOUNT, sizeof(LADSPA_PortDescriptor))) == NULL)
        exit(1);

    stereo_descriptor->PortDescriptors = (const LADSPA_PortDescriptor *)port_descriptors;
    port_descriptors[DIRECTION] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    port_descriptors[OCTAVE] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    port_descriptors[INPUTL] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    port_descriptors[INPUTR] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    port_descriptors[OUTPUTL] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    port_descriptors[OUTPUTR] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;

    if ((port_names =
        (char **)calloc(PORTCOUNT, sizeof(char *))) == NULL)
        exit(1);

    stereo_descriptor->PortNames = (const char **)port_names;
    port_names[DIRECTION] = strdup("Direction - High");
    port_names[OCTAVE] = strdup("Number of repetitions");
    port_names[INPUTL] = strdup("Input Left");
    port_names[INPUTR] = strdup("Input Right");
    port_names[OUTPUTL] = strdup("Output Left");
    port_names[OUTPUTR] = strdup("Output Right");

    if ((port_range_hints =
        ((LADSPA_PortRangeHint *)calloc(PORTCOUNT, sizeof(LADSPA_PortRangeHint)))) == NULL)
        exit(1);

    stereo_descriptor->PortRangeHints	= (const LADSPA_PortRangeHint *)port_range_hints;
    port_range_hints[DIRECTION].HintDescriptor =
    (LADSPA_HINT_BOUNDED_BELOW |
    LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_0);
    port_range_hints[OCTAVE].HintDescriptor =
    (LADSPA_HINT_BOUNDED_BELOW |
    LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_1);
    port_range_hints[DIRECTION].LowerBound = 0;
    port_range_hints[DIRECTION].UpperBound = 1;
    port_range_hints[OCTAVE].LowerBound = 0;
    port_range_hints[OCTAVE].UpperBound = 0.3;
    port_range_hints[INPUTL].HintDescriptor = 0;
    port_range_hints[INPUTR].HintDescriptor = 0;
    port_range_hints[OUTPUTL].HintDescriptor = 0;
    port_range_hints[OUTPUTR].HintDescriptor = 0;
    stereo_descriptor->instantiate = instantiate;
    stereo_descriptor->connect_port = connect_port;
    stereo_descriptor->activate = (void *) activate;
    stereo_descriptor->run = run;
    stereo_descriptor->deactivate = NULL;
    stereo_descriptor->cleanup = cleanup;
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
    switch (index) {
        case 0:
            return stereo_descriptor;
        default:
            return NULL;
    }
}

