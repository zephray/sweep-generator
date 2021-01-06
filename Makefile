CFLAGS = -O1 -g
LDLIBS = -lm

all: main

clean:
	rm main

main: main.c source.c waveheader.c