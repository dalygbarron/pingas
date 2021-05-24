#include "format.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 256

void formatRGBA(
    unsigned char const *in,
    unsigned int width,
    unsigned int height
) {
    unsigned char buffer[BUFFER_SIZE * 4];
    int cursor = 0;
    for (int i = 0; i < width * height; i++) {
        buffer[cursor] = in[i * 4];
        buffer[cursor + 1] = in[i * 4 + 1];
        buffer[cursor + 2] = in[i * 4 + 2];
        buffer[cursor + 3] = in[i * 4 + 3];
        cursor += 4;
        if (cursor == BUFFER_SIZE) {
            fwrite(buffer, sizeof(unsigned char), BUFFER_SIZE, stdout);
            cursor = 0;
        }
    }
    if (cursor > 0) {
        fwrite(buffer, sizeof(unsigned char), cursor, stdout);
    }
}

void formatRGB5_A1(float *in, unsigned int width, unsigned int height) {

}

void formatRGBA_2(
    unsigned char const *in,
    unsigned int width,
    unsigned int height
) {
    unsigned char buffer[BUFFER_SIZE];
    int cursor = 0;
    for (int i = 0; i < width * height; i++) {
        buffer[cursor] = in[i * 4] << 6 | in[i * 4 + 1] << 4 |
            in[i * 4 + 2] << 2 | in[i * 4 + 3];
        cursor++;
        if (cursor == BUFFER_SIZE) {
            fwrite(buffer, sizeof(unsigned char), BUFFER_SIZE, stdout);
            cursor = 0;
        }
    }
    if (cursor > 0) {
        fwrite(buffer, sizeof(unsigned char), cursor, stdout);
    }
}

void formatRGBA_1(
    unsigned char const *in,
    unsigned int width,
    unsigned int height
) {
    unsigned char buffer[BUFFER_SIZE];
    int cursor = 0;
    for (int i = 0; i < (width * height) / 2; i++) {
        buffer[cursor] = in[i * 8] << 7 | in[i * 8 + 1] << 6 |
            in[i * 8 + 2] << 5 | in[i * 8 + 3] << 4 | in[i * 8 + 4] << 3 |
            in[i * 8 + 5] << 2 | in[i * 8 + 6] << 1 | in[i * 8 + 7];
        cursor++;
        if (cursor == BUFFER_SIZE) {
            fwrite(buffer, sizeof(unsigned char), BUFFER_SIZE, stdout);
            cursor = 0;
        }
    }
    if (cursor > 0) {
        fwrite(buffer, sizeof(unsigned char), cursor, stdout);
    }
}

struct Format getFormat(char const *name) {
    struct Format format;
    format.red = 8;
    format.green = 8;
    format.blue = 8;
    format.alpha = 8;
    format.packer = &formatRGBA;
    if (strcmp(name, "RGBA") == 0) {
        // nothing lol.
    } else if (strcmp(name, "RGBA_2") == 0) {
        format.red = 2;
        format.green = 2;
        format.blue = 2;
        format.alpha = 2;
        format.packer = &formatRGBA_2;
    } else if (strcmp(name, "RGBA_1") == 0) {
        format.red = 1;
        format.green = 1;
        format.blue = 1;
        format.alpha = 1;
        format.packer = &formatRGBA_1;
    } else {
        fprintf(stderr, "%s is not a format I know\n", name);
    }
    return format;
}
