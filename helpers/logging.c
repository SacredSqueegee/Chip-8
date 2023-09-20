#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

void Log_Info(const char *format, ...)
{
    Print_Color(stdout, "[+] ", COLOR_GREEN);

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void Log_Warn(const char *format, ...)
{
    Print_Color(stdout, "[!] ", COLOR_YELLOW);

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void Log_Err(const char *format, ...)
{
    Print_Color(stderr, "[-] ", COLOR_RED);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

static void Print_Color(FILE *stream, const char *msg, const char *colorCode)
{
    fprintf(stream, "%s", colorCode);
    fprintf(stream, "%s", msg);
    fprintf(stream, "%s", COLOR_RESET); 
}
#define Print_Color cannot_call_Print_Color_outside_logging
