#ifndef FORMAT_H
#define FORMAT_H

/**
 * Represents a data output format.
 * the red blue green alpha numbers should be between 0 and 8.
 */
struct Format {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    void (*packer)(unsigned char const *, unsigned int, unsigned int);
};

/**
 * Takes the name of a format and gives you the format.
 * @param name the name of the format.
 */
struct Format getFormat(char const *name);

#endif
