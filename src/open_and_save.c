#include "ted.h"

void savefile() {
    FILE *fpw = fopen(filename, "w");

    if (fpw == NULL) {
        char buf[1000];
        snprintf(buf, 1000, "Could not open the file\nErrno: %d\nPress any key", errno);

        message(buf);

        return;
    }
    /*
    for (unsigned int i = 0; i < num_lines; i++) {
        fputs((const char *)lines[i].data, fpw);
        if (num_lines > 1) {
            if (config.line_break_type == 0)
                fputc('\n', fpw);
            else if (config.line_break_type == 1)
                fputs("\r\n", fpw);
            else if (config.line_break_type == 2)
                fputc('\r', fpw);
        }
    }
    */

    fclose(fpw);
}

void read_lines() {
    if (fp == NULL) {
        num_lines = 1;
        lines = malloc(sizeof(struct LINE));

        lines[0].len = READ_BLOCKSIZE;
        lines[0].data = malloc(lines[0].len);
        lines[0].length = 0;
        lines[0].data[0] = '\0';
        lines[0].ident = 0;

        return;
    }

    detect_linebreak();

    char lineend = config.line_break_type == 0 ? '\n' : '\r';

    num_lines = 0;
    for (unsigned int i = 0; !feof(fp); i++) {

        if (fgetc(fp) == EOF && num_lines > 0)
            break;
        else
            fseek(fp, -1, SEEK_CUR);

        lines = realloc(lines, ++num_lines * sizeof(struct LINE));

        lines[i].len = READ_BLOCKSIZE;
        lines[i].data = malloc(lines[i].len);
        lines[i].length = 0;
        lines[i].ident = 0;

        char c;
        unsigned int j;
        char passed_spaces = 0;

        for (j = 0; (c = fgetc(fp)) != lineend && c != EOF; j++) {

            if (j >= lines[i].len) {
                lines[i].len += READ_BLOCKSIZE;
                lines[i].data = realloc(lines[i].data, lines[i].len);
            }

            if (passed_spaces == 0 && c != ' ')
                passed_spaces = 1;
            else if (passed_spaces == 0)
                lines[i].ident++;

            unsigned char uc = *(unsigned char *)&c;

            utf8ReadFile(uc, j, fp);

            lines[i].length++;
        }

        if (j >= lines[i].len) {
            lines[i].len += READ_BLOCKSIZE;
            lines[i].data = realloc(lines[i].data, lines[i].len);
        }

        lines[i].data[j] = '\0';

        if (config.line_break_type == 1)
            fgetc(fp);
    }
}

void detect_linebreak() {
    char c;
    while (!feof(fp)) {
        c = fgetc(fp);

        if (c == '\r') {
            if (fgetc(fp) == '\n')
                config.line_break_type = 1;
            else
                config.line_break_type = 2;

            break;
        } else if (c == '\n') {
            config.line_break_type = 0;
            break;
        }
    }
    rewind(fp);
}
