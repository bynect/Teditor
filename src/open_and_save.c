#include "ted.h"

void savefile() {
    FILE *fpw = fopen(filename, "w");

    if (fpw == NULL) {
        char buf[1000];
        snprintf(buf, 1000, "Could not open the file\nErrno: %d\nPress any key", errno);

        message(buf);

        return;
    }
    
    for (unsigned int i = 0; i < num_lines; i++) {
        for (unsigned int j = 0; j < lines[i].length; j++) {
            unsigned char b[4];
            fwrite(b, sizeof(unsigned char), utf8ToMultibyte(lines[i].data[j], b), fpw);
        }
        if (num_lines > 1) {
            if (config.line_break_type == 0)
                fputc('\n', fpw);
            else if (config.line_break_type == 1)
                fputs("\r\n", fpw);
            else if (config.line_break_type == 2)
                fputc('\r', fpw);
        }
    }
    

    fclose(fpw);
}

void read_lines() {
    if (fp == NULL) {
        num_lines = 1;
        lines = malloc(sizeof(struct LINE));

        lines[0].len = READ_BLOCKSIZE;
        lines[0].data = malloc(lines[0].len * sizeof(uchar32_t));
        lines[0].color = malloc(lines[0].len * sizeof(unsigned char));
        lines[0].length = 0;
        lines[0].data[0] = '\0';
        lines[0].ident = 0;
        
        syntaxHighlight(0);

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
        lines[i].data = malloc(lines[i].len * sizeof(uchar32_t));
        lines[i].color = malloc(lines[i].len * sizeof(unsigned char));
        lines[i].length = 0;
        lines[i].ident = 0;
        syntaxHighlight(i);

        char c;
        unsigned int j;
        char passed_spaces = 0;

        for (j = 0; (c = fgetc(fp)) != lineend && c != EOF; j++) {

            if (j + 1 >= lines[i].len) {
                lines[i].len += READ_BLOCKSIZE;
                lines[i].data = realloc(lines[i].data, lines[i].len * sizeof(uchar32_t));
                lines[i].color = realloc(lines[i].color, lines[i].len * sizeof(unsigned char));
            }

            if (passed_spaces == 0 && c != ' ')
                passed_spaces = 1;
            else if (passed_spaces == 0)
                lines[i].ident++;

            unsigned char uc = *(unsigned char *)&c;

            utf8ReadFile(uc, j, i, fp);

            lines[i].length++;
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
