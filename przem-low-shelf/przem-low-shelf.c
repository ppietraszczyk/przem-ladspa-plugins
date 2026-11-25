/* przem-low-shelf - filtr półkowy dolnoprzepustowy
 * 17-11-2026
 * © Przemysław R. Pietraszczyk
 * licencja: MIT
 */
#include "przem-low-shelf.h"

static LADSPA_Handle instantiate(const LADSPA_Descriptor *descriptor,
                                 unsigned long sample_rate)
{
    LADSPA_Handle plugin;

    if ((plugin = calloc(1, sizeof(BassBoostPlugin))) != NULL) {
        ((BassBoostPlugin *)plugin)->sample_rate = sample_rate;
        return plugin;
    }
    return NULL;
}

static void connect_port(LADSPA_Handle handle, unsigned long port,
                         LADSPA_Data *value)
{
    BassBoostPlugin *plugin = (BassBoostPlugin *) handle;

    switch (port) {
        case GAIN:
            plugin->gain = value;
            break;
        case FREQ:
            plugin->freq = value;
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
    BassBoostPlugin *plugin = (BassBoostPlugin *) handle;
    plugin->Lx1 = plugin->Lx2 = plugin->Ly1 = plugin->Ly2 = 0.0;
    plugin->Rx1 = plugin->Rx2 = plugin->Ry1 = plugin->Ry2 = 0.0;

}

static void run(LADSPA_Handle handle, unsigned long SampleCount)
{
    BassBoostPlugin *plugin = (BassBoostPlugin *) handle;

    LADSPA_Data *inputl = plugin->inputl;
    LADSPA_Data *inputr = plugin->inputr;
    LADSPA_Data *outputl = plugin->outputl;
    LADSPA_Data *outputr = plugin->outputr;

    double fs = plugin->sample_rate;
    if (fs <= 0.0) return; // Zabezpieczenie przed błędnym sample_rate

    // Pobranie wartości z portów
    double gain = *(plugin->gain);
    double freq = *(plugin->freq);

    // 1. Zabezpieczenie przed wartością NaN / Inf (np. z uszkodzonego hosta)
    if (!isfinite(gain)) gain = 0.0;
    if (!isfinite(freq)) freq = 100.0;

    // 2. Ograniczenie zakresu (Clamping)
    if (gain < 0) gain = 0;
    else if (gain > 48.0) gain = 48.0;

    double max_freq = fs * 0.49; // Bezpieczny margines poniżej częstotliwości Nyquista (fs / 2)
    if (freq < 1.0) freq = 1.0;
    else if (freq > max_freq) freq = max_freq;

    // 3. Obliczenie współczynników filtra Biquad Low-Shelf
    double A = pow(10.0, gain / 40.0);
    double w0 = 2.0 * M_PI * freq / fs;
    double cos_w0 = cos(w0);
    double sin_w0 = sin(w0);
    double S = 1.0;

    // Zabezpieczenie przed liczbą ujemną pod pierwiastkiem (błędy zaokrągleń float)
    double radicand = (A + 1.0 / A) * (1.0 / S - 1.0) + 2.0;
    if (radicand < 0.0) radicand = 0.0;

    double alpha = sin_w0 / 2.0 * sqrt(radicand);
    double two_sqrtA_alpha = 2.0 * sqrt(A) * alpha;

    double b0 = A * ((A + 1.0) - (A - 1.0) * cos_w0 + two_sqrtA_alpha);
    double b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cos_w0);
    double b2 = A * ((A + 1.0) - (A - 1.0) * cos_w0 - two_sqrtA_alpha);
    double a0 = (A + 1.0) + (A - 1.0) * cos_w0 + two_sqrtA_alpha;
    double a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cos_w0);
    double a2 = (A + 1.0) + (A - 1.0) * cos_w0 - two_sqrtA_alpha;

    // 4. Zabezpieczenie przed dzieleniem przez zero przy normalizacji
    if (fabs(a0) < 1e-12) {
        // W razie degeneracji ustawiamy filtr w tryb bypass (przepuszcza dźwięk bez zmian)
        b0 = 1.0; b1 = 0.0; b2 = 0.0;
        a1 = 0.0; a2 = 0.0;
    } else {
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
    }

    // Pobranie stanu z poprzedniego bloku
    double Lx1 = plugin->Lx1, Lx2 = plugin->Lx2;
    double Ly1 = plugin->Ly1, Ly2 = plugin->Ly2;

    // 2. Pętla przetwarzająca każdą próbkę w buforze
    for (unsigned long i = 0; i < SampleCount; i++) {
        double Lx0 = (double)*(inputl++); //input[i];

        // Równanie różnicowe filtra
        double Ly0 = b0 * Lx0 + b1 * Lx1 + b2 * Lx2 - a1 * Ly1 - a2 * Ly2;

        // Przesunięcie próbek w pamięci
        Lx2 = Lx1;
        Lx1 = Lx0;
        Ly2 = Ly1;
        Ly1 = Ly0;

        double out = Ly0;
        if (out > 1.0) out = 1.0;
        else if (out < -1.0) out = -1.0;

        //output[i] = (LADSPA_Data) y0;
        *(outputl++) = (LADSPA_Data) out; //out;
    }

    // Pobranie stanu z poprzedniego bloku
    double Rx1 = plugin->Rx1, Rx2 = plugin->Rx2;
    double Ry1 = plugin->Ry1, Ry2 = plugin->Ry2;
    // 2. Pętla przetwarzająca każdą próbkę w buforze
    for (unsigned long i = 0; i < SampleCount; i++) {
        double Rx0 = (double)*(inputr++); //input[i];

        // Równanie różnicowe filtra
        double Ry0 = b0 * Rx0 + b1 * Rx1 + b2 * Rx2 - a1 * Ry1 - a2 * Ry2;

        // Przesunięcie próbek w pamięci
        Rx2 = Rx1;
        Rx1 = Rx0;
        Ry2 = Ry1;
        Ry1 = Ry0;

        double out = Ry0;
        if (out > 1.0) out = 1.0;
        else if (out < -1.0) out = -1.0;

        //output[i] = (LADSPA_Data) y0;
        *(outputr++) = (LADSPA_Data) out; //out;
    }
    // Zapisanie stanu dla kolejnego wywołania run()
    plugin->Lx1 = Lx1; plugin->Lx2 = Lx2;
    plugin->Ly1 = Ly1; plugin->Ly2 = Ly2;

    plugin->Rx1 = Rx1; plugin->Rx2 = Rx2;
    plugin->Ry1 = Ry1; plugin->Ry2 = Ry2;
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
    /* compute the log10 table */
    //for (i = 0; i < 9000; i++)
    //    log10_table[i] = log10f(1.0f + i / 1000.0f);

    stereo_descriptor->UniqueID = ID_STEREO;
    stereo_descriptor->Label = strdup("Przem Low Shelf");
    stereo_descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE; //0;
    stereo_descriptor->Name = strdup("Przem Low Shelf");
    stereo_descriptor->Maker = strdup("Przemek R. Pietraszczyk");
    stereo_descriptor->Copyright = strdup("MIT");
    stereo_descriptor->PortCount = PORTCOUNT;

    if ((port_descriptors =
        (LADSPA_PortDescriptor *)calloc(PORTCOUNT, sizeof(LADSPA_PortDescriptor))) == NULL)
        exit(1);

    stereo_descriptor->PortDescriptors = (const LADSPA_PortDescriptor *)port_descriptors;
    port_descriptors[GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    port_descriptors[FREQ] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    port_descriptors[INPUTL] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    port_descriptors[INPUTR] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    port_descriptors[OUTPUTL] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    port_descriptors[OUTPUTR] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;

    if ((port_names =
        (char **)calloc(PORTCOUNT, sizeof(char *))) == NULL)
        exit(1);

    stereo_descriptor->PortNames = (const char **)port_names;
    port_names[GAIN] = strdup("Gain Level [dB]");
    port_names[FREQ] = strdup("Frequency [Hz]");
    port_names[INPUTL] = strdup("Input Left");
    port_names[INPUTR] = strdup("Input Right");
    port_names[OUTPUTL] = strdup("Output Left");
    port_names[OUTPUTR] = strdup("Output Right");

    if ((port_range_hints =
        ((LADSPA_PortRangeHint *)calloc(PORTCOUNT, sizeof(LADSPA_PortRangeHint)))) == NULL)
        exit(1);

    stereo_descriptor->PortRangeHints	= (const LADSPA_PortRangeHint *)port_range_hints;
    port_range_hints[GAIN].HintDescriptor =
    (LADSPA_HINT_BOUNDED_BELOW |
    LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_LOW);
    port_range_hints[FREQ].HintDescriptor =
    (LADSPA_HINT_BOUNDED_BELOW |
    LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_MIDDLE |
    LADSPA_HINT_LOGARITHMIC); // suwak będzie działał znacznie płynniej dla ucha);
    port_range_hints[GAIN].LowerBound = 0;
    port_range_hints[GAIN].UpperBound = 48;
    port_range_hints[FREQ].LowerBound = 20;
    port_range_hints[FREQ].UpperBound = 600;
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

