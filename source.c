#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Generate a sine wave with specified frequency (in Hz), duration (in sec), and sample rate (in Hz).
void generateSine(int16_t *buffer16, int32_t *buffer32, double amplitude, 
        double frequency, double duration, double sampleRate) {
    int samples = (int)(duration * sampleRate);
    for (int i = 0; i < samples; i++) {
        double time = fmod((double)i / sampleRate * frequency, 1.0) * 2.0 * M_PI;
        int32_t sample32 = (int32_t)(amplitude * sin(time) * 2147483647.0);
        buffer32[i * 2] = sample32;
        buffer32[i * 2 + 1] = sample32;
        int16_t sample16 = sample32 >> 16;
        buffer16[i * 2] = sample16;
        buffer16[i * 2 + 1] = sample16;
    }
}
