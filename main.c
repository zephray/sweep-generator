#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "source.h"
#include "waveheader.h"

#define SAMPLE_RATE (48000)

#define SWEEP_LIN 0
#define SWEEP_LOG 1

// The first step will be repeated to give audio analyzer additional time to
// set the input gain
#define REPEAT_FIRST

void generate(int method, double start_freq, double end_freq, int step_length, 
        int steps, double amplitude, char *filename) {
#ifdef REPEAT_FIRST
    steps++;
#endif
    int duration = (steps * step_length);
    int total_samples = (duration * SAMPLE_RATE);
    int data_size_16 = (total_samples * (16 / 8) * 2);
    int data_size_32 = data_size_16 * 2;

    int16_t *samples16 = malloc(data_size_16);
    int32_t *samples32 = malloc(data_size_32);

    double freq = start_freq;
    for (int i = 0; i < steps; i++) { 
        printf("%f Hz\n", freq);
        if (freq == 0.0) {
            memset(&(samples16[i * step_length * SAMPLE_RATE * 2]), 0, 
                    step_length * SAMPLE_RATE * 2);
            memset(&(samples32[i * step_length * SAMPLE_RATE * 2]), 0, 
                    step_length * SAMPLE_RATE * 2);
        }
        else {
            generateSine(&(samples16[i * step_length * SAMPLE_RATE * 2]), 
                    &(samples32[i * step_length * SAMPLE_RATE * 2]),
                    amplitude, (double)freq, (double)step_length, SAMPLE_RATE);
        }
#ifdef REPEAT_FIRST
        if (i == 0)
            continue;
#endif
        if (method == SWEEP_LIN)
            freq += (end_freq - start_freq) / (steps - 1);
        else if (method == SWEEP_LOG)
            freq *= 1.26; // Third-octave 
    }

    uint8_t header[44];

    waveheader(header, SAMPLE_RATE, 16, total_samples);

    char fn[128];
    FILE *fp;
    strcpy(fn, filename);
    strcat(fn, "_16.wav");
    fp = fopen(fn, "wb+");
    fwrite(header, 44, 1, fp);
    fwrite(samples16, data_size_16, 1, fp);
    fclose(fp);

    waveheader(header, SAMPLE_RATE, 32, total_samples);

    strcpy(fn, filename);
    strcat(fn, "_32.wav");
    fp = fopen(fn, "wb+");
    fwrite(header, 44, 1, fp);
    fwrite(samples32, data_size_32, 1, fp);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    
    printf("Sweep Generator\n");

    // Output amplitude at full-scale
    // 0 dBFS 997 Hz
    generate(SWEEP_LOG, 997.0, 0.0, 20, 1, 1.0, "1_amplitude");

    // Frequency response
    // -20 dBFS from 10 Hz to 20 kHz in third-octave steps
    generate(SWEEP_LOG, 10.0, 0.0, 2, 34, 0.1, "2_freqresp");

    // THD+N vs. frequency
    // -1 dBFS from 20 Hz to 10 kHz in third-octave steps
    generate(SWEEP_LOG, 20.0, 0.0, 2, 28, 0.891251, "3_thdn");

    // Idle channel noise
    // Silence
    generate(SWEEP_LOG, 0.0, 0.0, 20, 1, 1.0, "4_silence");

    // SNR
    // -60 dBFS 997 Hz
    generate(SWEEP_LOG, 997.0, 0.0, 20, 1, 0.001, "5_snr");

    return 0;
}