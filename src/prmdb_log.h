/*
** NAME: log
**
** DESC: Logging module. Logs to console and file.
**/


#ifndef _PRMDB_LOG_H
#define _PRMDB_LOG_H

/*------------------------------------------------------------------------
                                 LITERALS
------------------------------------------------------------------------*/

#define PRMDB_LOG_DEBUG 30
#define PRMDB_LOG_INFO  20
#define PRMDB_LOG_FATAL 10

/*------------------------------------------------------------------------
                                 FUNCTIONS
------------------------------------------------------------------------*/

void log_cleanup();

void log_debug(const char *msg, ...);

void log_fatal(const char *msg, ...);

void log_info(const char *msg, ...);

int log_init(int log_level);


#endif /* _PRMDB_LOG_H */