#include "ted.h"

void expandLine(unsigned int at, int x) {
    while (lines[at].len <= lines[at].length + x) {
        lines[at].len += READ_BLOCKSIZE;
        lines[at].data = realloc(lines[cy].data, lines[cy].len * sizeof(uchar32_t));
        lines[at].color = realloc(lines[cy].color, lines[cy].len * sizeof(unsigned char));
        syntaxHighlight(at);
    }
}

void process_keypress(int c) {
    switch (c) {
    case KEY_UP:
    case ctrl('p'):
        cursor.y -= cursor.y > 0;

        cursor.x = last_cursor_x;
        
        cursor_in_valid_position();
        break;
    case KEY_DOWN:
    case ctrl('n'):
        cursor.y++;

        cursor.x = last_cursor_x;
        
        cursor_in_valid_position();
        break;
    case KEY_LEFT:
    case ctrl('b'):
        cursor.x -= (cursor.x > 0);
        cursor_in_valid_position();
        last_cursor_x = cx;
        break;
    case KEY_RIGHT:
    case ctrl('f'):
        cursor.x++;
        cursor_in_valid_position();
        last_cursor_x = cx;
        break;
    case KEY_HOME:
    case ctrl('a'):
        cursor.x = 0;
        last_cursor_x = cx;
        cursor_in_valid_position();
        break;
    case KEY_END:
    case ctrl('e'):
        cursor.x = lines[cursor.y].length;
        last_cursor_x = cx;
        cursor_in_valid_position();
        break;
    case ctrl('s'):
        savefile();
        break;
    case '\t':
        if (config.use_spaces == 1) {
            for (unsigned int i = 0; i < config.tablen; i++)
                process_keypress(' ');
            return;
        } // else, it will pass though and be added to the buffer
        break;
    case ctrl('g'):
        config_dialog();
        break;
    case KEY_PPAGE: {
        unsigned int ccy = cy;
        for (unsigned int i = 0; i < (unsigned int)(ccy % config.LINES + config.LINES); i++)
            process_keypress(KEY_UP);
        break;
    } case KEY_NPAGE: {
        unsigned int ccy = cy;
        for (unsigned int i = 0; i < (unsigned int)(config.LINES - (ccy % config.LINES) - 1 + config.LINES); i++)
            process_keypress(KEY_DOWN);
        break;
    } case KEY_MOUSE: {
        MEVENT event;
        if (getmouse(&event) == OK)
            processMouseEvent(event);
        
        break;
    } case 0x209: {
        if (num_lines > 1) {
            free(lines[cy].data);
            free(lines[cy].color);
            memmove(&lines[cy], &lines[cy + 1], (num_lines - cy - 1) * sizeof(struct LINE));
            lines = realloc(lines, --num_lines * sizeof(struct LINE));
        } else {
            lines[cy].data[0] = '\0';
            lines[cy].length = 0;
        }
        cursor_in_valid_position();
        syntaxHighlight(cy);
        break;
    } case ctrl('w'): { // Calling 'syntaxHighlight' is not needed here because calling 'process_keypress(KEY_BACKSPACE)' does it
        bool passed_spaces = 0;
        process_keypress(KEY_LEFT);
        while (!strchr(config.word_separators, lines[cy].data[cx]) || !passed_spaces) {
            process_keypress(KEY_RIGHT);
            process_keypress(KEY_BACKSPACE);
            process_keypress(KEY_LEFT);
            if (!strchr(config.word_separators, lines[cy].data[cx]))
                passed_spaces = 1;
        }
        process_keypress(KEY_RIGHT);
        break;
    } case ctrl('o'): {
        char *d = prompt("open: ");
        if (d)
            openFile(d, 1);
        
        break;
    }
    }

    if (c == CTRL_KEY_LEFT) {
        char passed_spaces = 0;
        while (cx > 0) {
            process_keypress(KEY_LEFT);
            if (!strchr(config.word_separators, lines[cy].data[cx]))
                passed_spaces = 1;
            if (strchr(config.word_separators, lines[cy].data[cx]) && passed_spaces) {
                process_keypress(KEY_RIGHT);
                break;
            }
        }
    } else if (c == CTRL_KEY_RIGHT) {
        char passed_spaces = 0;
        while (lines[cy].data[cx] != '\0' && !(strchr(config.word_separators, lines[cy].data[cx]) && passed_spaces)) {
            if (!strchr(config.word_separators, lines[cy].data[cx]))
                passed_spaces = 1;
            process_keypress(KEY_RIGHT);
        }
    } else if (isprint(c) || c == '\t' || (c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xEF) || (c >= 0xF0 && c <= 0xF7)) {
        if (c == ' ' && cx <= lines[cy].ident)
            lines[cy].ident++;

        expandLine(cy, 1);

        memmove(&lines[cy].data[cx + 1], &lines[cy].data[cx], (lines[cy].length - cx) * sizeof(uchar32_t));

        lines[cy].data[cx] = c;
        
        if ((c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xEF) || (c >= 0xF0 && c <= 0xF7))
            lines[cy].data[cx] += getch() << 8;
        
        if ((c >= 0xE0 && c <= 0xEF) || (c >= 0xF0 && c <= 0xF7))
            lines[cy].data[cx] += getch() << 16;

        if (c >= 0xF0 && c <= 0xF7)
            lines[cy].data[cx] = getch() << 24;
        
        lines[cy].data[++lines[cy].length] = '\0';
        syntaxHighlight(cy);
        process_keypress(KEY_RIGHT);
    } else if (c == KEY_BACKSPACE || c == KEY_DC || c == 127) {
        if (cx <= lines[cy].ident && cx > 0)
            lines[cy].ident--;

        if (cx >= 1) {
            memmove(&lines[cy].data[cx - 1], &lines[cy].data[cx], (lines[cy].length - cx) * sizeof(uchar32_t));
            lines[cy].data[--lines[cy].length] = '\0';

            process_keypress(KEY_LEFT);
        } else if (cy > 0) {
            uchar32_t *del_line = lines[cy].data;
            unsigned char *del_line_color = lines[cy].color;
            unsigned int del_line_len = lines[cy].length;

            memmove(&lines[cy], &lines[cy + 1], (num_lines - cy - 1) * sizeof(struct LINE));
            
            lines = realloc(lines, --num_lines * sizeof(struct LINE));

            process_keypress(KEY_UP);

            cursor.x = lines[cy].length;
            
            process_keypress(KEY_RIGHT);

            expandLine(cy, del_line_len);

            memmove(&lines[cy].data[lines[cy].length], del_line, del_line_len * sizeof(uchar32_t));
            lines[cy].length += del_line_len;

            lines[cy].data[lines[cy].length] = '\0';

            free(del_line);
            free(del_line_color);
        }

        lines[cy].ident = 0;
        for (unsigned int i = 0; lines[cy].data[i] != '\0'; i++) {
            if (lines[cy].data[i] != ' ')
                break;
            lines[cy].ident++;
        }
        syntaxHighlight(cy);
    } else if (c == '\n' || c == KEY_ENTER || c == '\r') {
        lines = realloc(lines, (num_lines + 1) * sizeof(struct LINE));
    
        memmove(&lines[cy + 2], &lines[cy + 1], (num_lines - cy - 1) * sizeof(struct LINE));

        num_lines++;

        unsigned int lcx = cx;
        cursor.x = 0;
        last_cursor_x = 0;
        process_keypress(KEY_DOWN);

        lines[cy].len = READ_BLOCKSIZE;
        lines[cy].data = malloc(lines[cy].len * sizeof(uchar32_t));
        lines[cy].color = malloc(lines[cy].len * sizeof(unsigned char));
        lines[cy].length = 0;
        
        expandLine(cy, lines[cy - 1].length - lcx + 1);
        
        memcpy(lines[cy].data, &lines[cy - 1].data[lcx], (lines[cy - 1].length - lcx) * sizeof(uchar32_t));
        
        lines[cy].length += lines[cy - 1].length - lcx;
        
        lines[cy].data[lines[cy].length] = '\0';

        lines[cy - 1].length = lcx;

        lines[cy - 1].data[lines[cy - 1].length] = '\0';

        char tmp[50];
        len_line_number = snprintf(tmp, 50, "%u", num_lines + 1);

        if (config.autotab == 1) {
            const unsigned int ident = lines[cy - 1].ident;
            lines[cy].ident = ident;
            lines[cy].len += ident + 1;
            lines[cy].data = realloc(lines[cy].data, lines[cy].len * sizeof(uchar32_t));
            lines[cy].color = realloc(lines[cy].color, lines[cy].len * sizeof(unsigned char));
            memmove(&lines[cy].data[ident], lines[cy].data, (lines[cy].length + 1) * sizeof(uchar32_t));

            for (unsigned int i = 0; i < ident; i++)
                lines[cy].data[i] = ' ';
            lines[cy].length += ident;

            for (unsigned int i = 0; i < ident; i++)
                process_keypress(KEY_RIGHT);
        }
        else
            lines[cy].ident = 0;

        syntaxHighlight(cy);
        syntaxHighlight(cy - 1);
    }
}
