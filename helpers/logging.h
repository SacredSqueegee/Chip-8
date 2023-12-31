#ifndef LOGGING_H_IRISH
#define LOGGING_H_IRISH

#include <stdio.h>
#include <stdarg.h>

// Terminal colors via escape codes
#define COLOR_BLACK "\033[0;30m"
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_PURPLE "\033[0;35m"
#define COLOR_CYAN "\033[0;36m"
#define COLOR_WHITE "\033[0;37m"

#define COLOR_RESET "\033[0m"

// These prepend a colored log symbol to our log msg 
int Log_Info( const char* format, ... );
int Log_Warn( const char* format, ... );
int Log_Err( const char* format, ... );


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

// Used to write 'msg' with the specified 'colorCode'
static void Print_Color(FILE *stream, const char *msg, const char *colorCode);

#pragma clang diagnostic pop


#endif