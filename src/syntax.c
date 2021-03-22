#include "syntax.h"

/*
C and C++ syntax highlighting descriptor
*/

static struct KWD c_cpp_kwd[] = {
    {"if", 0x10}, {"else", 0x10}, {"while", 0x10}, {"for", 0x10},
    {"int", 0x20}, {"char", 0x20}, {"unsigned", 0x20}, {"double", 0x20},
    {"float", 0x20}, {"struct", 0x20}, {"const", 0x20}, {"return", 0x20},
    {"void", 0x20},
    {"*", 0x30}, {";", 0x30}, {",", 0x30}
};

static const char *c_cpp_exts[] = {"c", "h", "cpp", "hpp", "cc", "hh"};

static struct SHD c_cpp_syntax = {
    sizeof c_cpp_exts / sizeof *c_cpp_exts, c_cpp_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof c_cpp_kwd / sizeof *c_cpp_kwd, c_cpp_kwd, //Keywords
    0x40, 0x50, 0x05,
    "\"\'`", // Strings charaters
    "//", {"/*", "*/"}, // Comments
    {"{[(", ")]}"}
};

/*
Python syntax highlighting descriptor
*/

static struct KWD python_kwd[] = {
    {"if", 0x10}, {"else", 0x10}, {"elif", 0x10}, {"while", 0x10},
    {"for", 0x10}, {"in", 0x10}, {"with", 0x10}, {"class", 0x10},
    {"None", 0x20}, {"False", 0x20}, {"True", 0x20}, {"return", 0x20},
    {"*", 0x30}, {";", 0x30}, {",", 0x30}
};

static const char *python_exts[] = {"py"};

static struct SHD python_syntax = {
    sizeof python_exts / sizeof *python_exts, python_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof python_kwd / sizeof *python_kwd, python_kwd, //Keywords
    0x40, 0x50, 0x05,
    "\"\'`", // Strings charaters
    "#", {"\"\"\"", "\"\"\""}, // Comments
    {"{[(", ")]}"}
};


/*
Global syntaxes
*/
struct SHD *syntaxes[] = {&c_cpp_syntax, &python_syntax};

void register_syntax(void) {
    config.syntaxes = syntaxes;
    config.syntax_len = sizeof syntaxes / sizeof *syntaxes;
}
