#include "ted.h"

struct LINE *lines = NULL;
unsigned int num_lines;
unsigned int len_line_number; // The length of the number of the last line

FILE *fp = NULL;
struct CURSOR cursor = {0, 0};
struct TEXT_SCROLL text_scroll = {0, 0};

char *filename;
char *menu_message = "";

bool colors_on = 0;
bool needs_to_free_filename;

void setcolor(int c) {
    if (colors_on)
        attrset(c);
}

unsigned int last_cursor_x = 0;

struct KWD kwd[] = {
    {"if", 0x10}, {"else", 0x10},
    {"int", 0x20}, {"char", 0x20}, {"unsigned", 0x20}, {"double", 0x20}, {"float", 0x20}, {"struct", 0x20}, {"const", 0x20}, {"return", 0x20},
    {"*", 0x30}, {";", 0x30},
    {"(", 0x04}, {")", 0x04},
    {"\"", 0x40}, {"\'", 0x40},
    {"//", 0x50}, {"/*", 0x50}, {"*/", 0x50}
};

struct CFG config = {
    4, 0, 0, 1, 1,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    0, // kwdlen is automatically calculated
    kwd
};

unsigned int display_cx() {
    unsigned int ret = cx;
    for (unsigned int i = 0; i < cx; i++)
        if (lines[cy].data[i] == '\t')
            ret += config.tablen - 1;
    return ret;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        struct stat st = {0};

        char *config = home_path(".config/");
        if (stat(config, &st) == -1)
            mkdir(config, 0777);

        char *config_ted = home_path(".config/ted/");
        if (stat(config_ted, &st) == -1)
            mkdir(config_ted, 0777);

        filename = home_path(".config/ted/buffer");
        
        strcpy(filename, config_ted);
        strcat(filename, "buffer");

        needs_to_free_filename = 1;
        free(config);
        free(config_ted);
    } else {
        filename = argv[1];
        needs_to_free_filename = 0;
    }
    
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(1);
    
    config.kwdlen = sizeof kwd / sizeof *kwd;
    
    fp = fopen(filename, "r");

    read_lines();
    if (fp != NULL)
        fclose(fp);

    char tmp[50];
    len_line_number = snprintf(tmp, 50, "%u", num_lines + 1);


    if (has_colors() && start_color() == OK)
        colors_on = 1;

    if (colors_on) {
        use_default_colors();
        init_pair(1, COLOR_RED, -1);
        init_pair(2, -1, -1);
    }
    

    int c;
    while (1) {
        curs_set(0);
        config.LINES = LINES - 1;
        
        show_lines();
        show_menu(menu_message);
        move(cursor.y - text_scroll.y, display_cx() - text_scroll.x + len_line_number + 1);
        refresh();
        curs_set(1);

        c = getch();

        if (c == ctrl('c'))
            break;

        process_keypress(c);
    }

    free_lines();

    if (needs_to_free_filename == 1)
        free(filename);

    endwin();
    return 0;
}
