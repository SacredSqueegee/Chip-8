#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

void Log_Info(const char *format, ...)
{
    Print_Color(stdout, "[+] ", COLOR_GREEN);

    va_list args;
    va_start(args, format);
    Log(stdout, format, args);
    va_end(args);
}

void Log_Warn(const char *format, ...)
{
    Print_Color(stdout, "[!] ", COLOR_YELLOW);

    va_list args;
    va_start(args, format);
    Log(stdout, format, args);
    va_end(args);
}

void Log_Err(const char *format, ...)
{
    Print_Color(stderr, "[-] ", COLOR_RED);

    va_list args;
    va_start(args, format);
    Log(stderr, format, args);
    va_end(args);
}

static void Log(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
    fprintf(stream, "\n");
}
#define Log cannot_call_Log_ourside_logging

static void Print_Color(FILE *stream, const char *msg, const char *colorCode)
{
    fprintf(stream, "%s", colorCode);
    fprintf(stream, "%s", msg);
    fprintf(stream, "%s", COLOR_RESET); 
}
#define Print_Color cannot_call_Print_Color_outside_logging
