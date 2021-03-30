#include "ted.h"

const unsigned char invalid1 = 0xEF;
const unsigned char invalid2 = 0xBF;
const unsigned char invalid3 = 0xBD;

void utf8ReadFile(unsigned char uc, unsigned int lc, unsigned int i, FILE *fp_) {
    if (uc <= 0x7F) {
        lines[i].data[lc] = uc;
    } else if (uc >= 0xC0 && uc <= 0xDF) {
        lines[i].data[lc] = uc;
        lines[i].data[lc] += (unsigned int)fgetc(fp_) << 8;
    } else if (uc >= 0xE0 && uc <= 0xEF) {
        lines[i].data[lc] = uc;
        lines[i].data[lc] += (unsigned int)fgetc(fp_) << 8;
        lines[i].data[lc] += (unsigned int)fgetc(fp_) << 16;
    } else if (uc >= 0xF0 && uc <= 0xF7) {
        lines[i].data[lc] = uc;
        lines[i].data[lc] += (unsigned int)fgetc(fp_) << 8;
        lines[i].data[lc] += (unsigned int)fgetc(fp_) << 16;
        lines[i].data[lc] += (unsigned int)fgetc(fp_) << 24;
    } else {
        lines[i].data[lc] = invalid1;
        lines[i].data[lc] += (unsigned int)invalid2 << 8;
        lines[i].data[lc] += (unsigned int)invalid3 << 16;
    }
}

uint16_t utf8ToMultibyte(uchar32_t c, unsigned char *out) {
    out[0] = (unsigned char)(c % (1 << 8));
    out[1] = (unsigned char)((c >> 8) % (1 << 8));
    out[2] = (unsigned char)((c >> 16) % (1 << 8));
    out[3] = (unsigned char)(c >> 24);

    if (out[0] <= 0x7F)
        return 1;
    if (out[0] >= 0xC0 && out[0] <= 0xDF)
        return 2;
    if (out[0] >= 0xE0 && out[0] <= 0xEF)
        return 3;
    if (out[0] >= 0xF0 && out[0] <= 0xF7)
        return 4;

    out[0] = invalid1;
    out[1] = invalid2;
    out[2] = invalid3;
    return 3;
}
