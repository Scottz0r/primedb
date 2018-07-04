/*
** NAME: assert
**
** DESC: Assertions to catch program and logic errors.
**/
#ifndef _PRMDB_ASSERT_H
#define _PRMDB_ASSERT_H

/*------------------------------------------------------------------------
                                LITERALS
------------------------------------------------------------------------*/

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

/*------------------------------------------------------------------------
                                MACROS
------------------------------------------------------------------------*/

#define PRMDB_ASSERT_FATAL(expr, msg) (void)(!!(expr) || (_assert_fatal(#expr, msg, __FILE__, __func__, __LINE__), 0))

#ifdef _DEBUG
#define PRMDB_ASSERT_DEBUG(expr, msg) (void)(!!(expr) || (_assert_fatal(#expr, msg, __FILE__, __func__, __LINE__), 0))
#else
#define PRMDB_ASSERT_DEBUG(expr, msg)  ((void)0)
#endif

/*------------------------------------------------------------------------
                               FUNCTIONS
------------------------------------------------------------------------*/

void _assert_fatal(const char *expr, const char *msg, const char *file, const char *func, int line);

void _assert_debug(const char *expr, const char *msg, const char *file, const char *func, int line);

#endif /* _PRMDB_ASSERT_H */