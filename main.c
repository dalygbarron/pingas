#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "format.h"
#include "lodepng.h"

/*
 * The process should go like this:
 *  - parse commandline crap and load in the image file which is converted to
 *    rgba sequence of floats
 *  - sequence of floats is passed to a dithering function which converts it
 *    an rgba sequence of unsigned characters. each character has it's own byte
 *    at this point even if it is actually meant to occupy less space. number
 *    of bits per colour is based on output format.
 *  - sequence of bytes is passed to function that packs it to the format it
 *    will take in the outside world and outputs it.
 */

void help(char const *name) {
    printf("Pingas copyright 2021 Daly Graham Barron\n");
    printf("Released under GPL 2 license\n");
    printf("Takes a png image and decodes it to pixel data then writes that ");
    printf("pixel data out to stdout.\n");
    printf("usage: %s [options] filename\n", name);
    printf("options:\n");
    printf("  -f FORMAT    specifies pixel format, must be RGBA|RGB5_A1|RGBA_2\n");
    printf("  -h           displays this help message\n");
}

unsigned char cast(float in, unsigned int max) {
    if (in < 0) in = 0;
    if (in > 1) in = 1;
    return round(in * max);
}

void put(float *at, float r, float g, float b, float a, float power) {
    at[0] += r * power;
    at[1] += g * power;
    at[2] += b * power;
    at[3] += a * power;
}

void dither(
    float *in,
    unsigned char *out,
    unsigned int width,
    unsigned int height,
    struct Format const *format
) {
    int rP = (1 << format->red) - 1;
    int gP = (1 << format->green) - 1;
    int bP = (1 << format->blue) - 1;
    int aP = (1 << format->alpha) - 1;
    float errorPower = 0.0625;
    int max = width * height;
    for (int i = 0; i < max; i++) {
        float qR = round(in[i * 4] * rP) / rP;
        float qG = round(in[i * 4 + 1] * gP) / gP;
        float qB = round(in[i * 4 + 2] * bP) / bP;
        float qA = round(in[i * 4 + 3] * aP) / aP;
        float eR = (in[i * 4] - qR) * errorPower;
        float eG = (in[i * 4 + 1] - qG) * errorPower;
        float eB = (in[i * 4 + 2] - qB) * errorPower;
        float eA = (in[i * 4 + 3] - qA) * errorPower;
        int right = (i + 1) *  4;
        int bottomLeft = (i + width - 1) * 4;
        int bottom = (i + width) * 4;
        int bottomRight = (i + width + 1) * 4;
        if (right < max * 4) put(in + right, eR, eG, eB, eA, 7);
        if (bottomLeft < max * 4) put(in + bottomLeft, eR, eG, eB, eA, 3);
        if (bottom < max * 4) put(in + bottom, eR, eG, eB, eA, 5);
        if (bottomRight < max * 4) put(in + bottomRight, eR, eG, eB, eA, 1);
        out[i * 4] = cast(qR, rP);
        out[i * 4 + 1] = cast(qG, gP);
        out[i * 4 + 2] = cast(qB, bP);
        out[i * 4 + 3] = cast(qA, aP);
    }
}

int main(int argc, char **argv) {
    char *filename;
    struct Format format = getFormat("RGBA");
    // Parse commandline options.
    int opt;
    while ((opt = getopt(argc, argv, "hf:")) != -1) {
        switch (opt) {
            case 'f':
                format = getFormat(optarg);
                break;
            case 'h':
                help(argv[0]);
                return 0;
        }
    }
    int remainingArgs = argc - optind;
    if (remainingArgs != 1) {
        fprintf(stderr, "One image file must be specified.\n");
        return 1;
    }
    filename = argv[optind];
    // Decode the picture
    unsigned char *img = 0;
    unsigned int width, height;
    unsigned int error = lodepng_decode32_file(
        &img,
        &width,
        &height,
        filename
    );
    if (error) {
        fprintf(
            stderr,
            "png error %u: %s\n",
            error,
            lodepng_error_text(error)
        );
        return 1;
    }
    // convert the format if needed.
    float in[width * height * 4];
    unsigned char out[width * height * 4];
    for (int i = 0; i < width * height; i++) {
        in[i * 4] = ((float)img[i * 4]) / 255;
        in[i * 4 + 1] = ((float)img[i * 4 + 1]) / 255;
        in[i * 4 + 2] = ((float)img[i * 4 + 2]) / 255;
        in[i * 4 + 3] = ((float)img[i * 4 + 3]) / 255;
    }
    free(img);
    dither(in, out, width, height, &format);
    format.packer(out, width, height);
}

