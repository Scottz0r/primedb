#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prmdb_assert.h"
#include "prmdb_log.h"

/*------------------------------------------------------------------------
                                 LITERALS
------------------------------------------------------------------------*/

/* Path seperator - platform dependent. */
#ifdef _WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

/*------------------------------------------------------------------------
                               FUNCTIONS
------------------------------------------------------------------------*/

void _assert_fatal(const char *expr, const char *msg, const char *file, const char *func, int line)
{
    const char *filename_only;

    /* Get the filename part only from file param. */
    filename_only = strrchr(file, PATH_SEP);

    if (filename_only == NULL)
    {
        filename_only = file;
    }

    // TODO: Use logging functionality.
    log_fatal("\n\n");
    log_fatal("FATAL ASSERT FAILED! \n");
    log_fatal("  Expression: %s \n", expr);
    log_fatal("  File: %s \n", filename_only);
    log_fatal("  Function: %s \n", func);
    log_fatal("  Line: %d \n", line);
    log_fatal("\n");
    log_fatal("%s \n", msg);

    log_fatal("\nTerminating Progarm.\n");

    exit(EXIT_FAILURE);
}

void _assert_debug(const char *expr, const char *msg, const char *file, const char *func, int line)
{
    const char *filename_only;

    /* Get the filename part only from file param. */
    filename_only = strrchr(file, PATH_SEP);

    if (filename_only == NULL)
    {
        filename_only = file;
    }

    // TODO: Use logging functionality.
    log_fatal("\n\n");
    log_fatal("DEBUG ASSERT FAILED! \n");
    log_fatal("  Expression: %s \n", expr);
    log_fatal("  File: %s \n", filename_only);
    log_fatal("  Function: %s \n", func);
    log_fatal("  Line: %d \n", line);
    log_fatal("\n");
    log_fatal("%s \n", msg);

}