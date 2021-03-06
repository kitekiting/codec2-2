/*---------------------------------------------------------------------------*\

  FILE........: c2demo.c
  AUTHOR......: David Rowe
  DATE CREATED: 15/11/2010

  Encodes and decodes a file of raw speech samples using Codec 2.
  Demonstrates use of Codec 2 function API.

  Note to convert a wave file to raw and vice-versa:

    $ sox file.wav -r 8000 -s -2 file.raw
    $ sox -r 8000 -s -2 file.raw file.wav

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2010 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include "codec2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BITS_SIZE	((CODEC2_BITS_PER_FRAME + 7) / 8)

int main(int argc, char *argv[])
{
    struct CODEC2 *codec2;
    FILE          *fin;
    FILE          *fout;
    short         *buf;
    unsigned char *bits;
    int            nsam, nbit;

    if (argc < 3 || argc > 4) {
	printf("usage: %s InputRawSpeechFile OutputRawSpeechFile 3200|2400|1400|1200 (bitrate is optional)\n", argv[0]);
	exit(EXIT_FAILURE);
    }
 
    if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input speech file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(EXIT_FAILURE);
    }

    if ( (fout = fopen(argv[2],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output speech file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(EXIT_FAILURE);
    }
    int mode = CODEC2_MODE_1400;
    if (argc == 4) {
        char *endptr;
        long int bitrate = strtol(argv[3], &endptr, 10);
        if (endptr[0] != '\0') {
            fprintf(stderr, "Error parsing the given bitrate: \"%s\". Invalid part is \"%s\".\n",
                    argv[3], endptr);
            exit(EXIT_FAILURE);
        }
        switch(bitrate) {
        case 3200:
            mode = CODEC2_MODE_3200;
            break;
        case 2400:
            mode = CODEC2_MODE_2400;
            break;
        case 1400:
            mode = CODEC2_MODE_1400;
            break;
        case 1200:
            mode = CODEC2_MODE_1200;
            break;
        default:
            fprintf(stderr, "Error parsing the given bitrate: \"%s\". It must be either 3200, 2400, 1400 or 1200.\n",
                    argv[3]);
            exit(EXIT_FAILURE);
        }
    }

    /* Note only one set of Codec 2 states is required for an encoder
       and decoder pair. */

    codec2 = codec2_create(mode);
    nsam = codec2_samples_per_frame(codec2);
    buf = (short*)malloc(nsam*sizeof(short));
    nbit = codec2_bits_per_frame(codec2);
    bits = (unsigned char*)malloc(nbit*sizeof(char));

    while(fread(buf, sizeof(short), nsam, fin) == (size_t)nsam) {
	codec2_encode(codec2, bits, buf);
	codec2_decode(codec2, buf, bits);
	fwrite(buf, sizeof(short), nsam, fout);
    }

    free(buf);
    free(bits);
    codec2_destroy(codec2);

    fclose(fin);
    fclose(fout);

    return 0;
}
