#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "prmdb_log.h"
#include "prmdb_types.h"

/*------------------------------------------------------------------------
                                 LITERALS
------------------------------------------------------------------------*/

#define LOG_FILE    "prmdb.txt"

#define LPFX_DEBUG  "DEBEG"
#define LPFX_INFO   "INFO "
#define LPFX_FATAL  "FATAL"

/*------------------------------------------------------------------------
                              STATIC VARIABLES
------------------------------------------------------------------------*/

static int      s_log_level;
static FILE     *s_log_fp;

/*------------------------------------------------------------------------
                          FUNCTION DECLARATIONS
------------------------------------------------------------------------*/

static void write_message(const char *log_prefix, const char *msg, const va_list args);

/*------------------------------------------------------------------------
                                FUNCTIONS
------------------------------------------------------------------------*/

void log_cleanup()
{
    int close_rc;

    if (s_log_fp != NULL)
    {
        close_rc = fclose(s_log_fp);
        if (close_rc != 0)
        {
            printf("Failed to close log file handle. \n");
        }
    }
}

void log_debug(const char *msg, ...)
{
    if (s_log_level >= PRMDB_LOG_DEBUG)
    {
        va_list args;
        va_start(args, msg);
        write_message(LPFX_DEBUG, msg, args);
        va_end(args);
    }
}

void log_fatal(const char *msg, ...)
{
    if (s_log_level >= PRMDB_LOG_FATAL)
    {
        va_list args;
        va_start(args, msg);
        write_message(LPFX_FATAL, msg, args);
        va_end(args);
    }
}

void log_info(const char *msg, ...)
{
    if (s_log_level >= PRMDB_LOG_INFO)
    {
        va_list args;
        va_start(args, msg);
        write_message(LPFX_INFO, msg, args);
        va_end(args);
    }
}

int log_init(int log_level)
{
    s_log_level = log_level;

    s_log_fp = fopen(LOG_FILE, "a+b");

    /* 
    ** Display a warning if the log file couldn't be opened. Logging shouldn't
    ** make the program critically fail, so fall back to console only.
    */
    if (s_log_fp == NULL)
    {
        printf("[LOG INTERNAL] - Failed to open log file. Falling back to console log only. \n\n");
    }

    return PRMDB_OK;
}

static void write_message(const char *log_prefix, const char *msg, const va_list args)
{
    static char time_buffer[50];

    int         flush_rc;
    time_t      rawtime;
    struct tm   *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("%s [%s] - ", time_buffer, log_prefix);
    vprintf(msg, args);
    printf("\n");

    /* Only log if file was initialized. */
    if (s_log_fp != NULL)
    {
        fprintf(s_log_fp, "%s [%s] - ", time_buffer, log_prefix);
        vfprintf(s_log_fp, msg, args);
        fprintf(s_log_fp, "\n");

        flush_rc = fflush(s_log_fp);
        if (flush_rc == EOF)
        {
            printf("[LOG INTERNAL] - Failed to flush log file!\n");
        }
    }
}