#include "ted.h"

struct CFG config = {
    4, 0, 0, 1, 1, 1,
    &default_syntax, 0, NULL
};

void save_config(char *fname) {
    FILE *fpw = fopen(fname, "w");
    char buf[1000];

    if (fpw == NULL) {
        snprintf(buf, 1000, "Could not open the file\nErrno: %d\nPress any key", errno);
        message(buf);
        return;
    }

    unsigned int len = snprintf(buf, 1000, "tablen = %d", config.tablen);
    fwrite(buf, sizeof(char), len, fpw);
    put_line_break(fpw);

    len = snprintf(buf, 1000, "line_break = %s", config.line_break_type == 0 ? "LF" : (config.line_break_type == 1 ? "CRLF" : "CR"));
    fwrite(buf, sizeof(char), len, fpw);
    put_line_break(fpw);

    len = snprintf(buf, 1000, "use_spaces = %s", config.use_spaces ? "true" : "false");
    fwrite(buf, sizeof(char), len, fpw);
    put_line_break(fpw);

    len = snprintf(buf, 1000, "autotab = %s", config.autotab ? "true" : "false");
    fwrite(buf, sizeof(char), len, fpw);
    put_line_break(fpw);

    len = snprintf(buf, 1000, "insert_newline = %s", config.insert_newline ? "true" : "false");
    fwrite(buf, sizeof(char), len, fpw);
    put_line_break(fpw);

    fclose(fpw);
}

void load_config(char *fname) {
    FILE *fpr = fopen(fname, "r");
    char buf[1000];

    unsigned int tablen;
    char buf2[10];

    while (fgets(buf, 1000, fpr)) {
        if (*buf == '#' || !*buf)
            continue;

        if (sscanf(buf, "tablen = %u", &tablen) != -1) {
            config.tablen = tablen;

        } else if (sscanf(buf, "line_break = %s", buf2) != -1) {
            if (strcasecmp(buf2, "LF") == 0)
                config.line_break_type = 0;
            else if (strcasecmp(buf2, "CRLF") == 0)
                config.line_break_type = 1;
            else if (strcasecmp(buf2, "CR") == 0)
                config.line_break_type = 2;
        
        } else if (sscanf(buf, "use_spaces = %s", buf2) != -1) {
            if (strcasecmp(buf2, "TRUE") == 0 || strcmp(buf2, "1") == 0)
                config.use_spaces = 1;
            else if (strcasecmp(buf2, "FALSE") == 0 || strcmp(buf2, "0") == 0)
                config.use_spaces = 0;

        } else if (sscanf(buf, "autotab = %s", buf2) != -1) {
            if (strcasecmp(buf2, "TRUE") == 0 || strcmp(buf2, "1") == 0)
                config.autotab = 1;
            else if (strcasecmp(buf2, "FALSE") == 0 || strcmp(buf2, "0") == 0)
                config.autotab = 0;

        } else if (sscanf(buf, "insert_newline = %s", buf2) != -1) {
            if (strcasecmp(buf2, "TRUE") == 0 || strcmp(buf2, "1") == 0)
                config.insert_newline = 1;
            else if (strcasecmp(buf2, "FALSE") == 0 || strcmp(buf2, "0") == 0)
                config.insert_newline = 0;
        }
    }
    
    fclose(fpr);
}
