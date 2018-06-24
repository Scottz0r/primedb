/*
** NAME: dbfile
**
** DESC: Holds prime data and writes to prime data file.
**/
#ifndef _PRMDB_DBFILE_H
#define _PRMDB_DBFILE_H

#include "prmdb_types.h"

/*------------------------------------------------------------------------
                               LITERALS
------------------------------------------------------------------------*/

#define MAX_PRIME_ARRAY     52428800 /* 200 MB of 32-bit integers. */

/*------------------------------------------------------------------------
                           EXTERN VARIABLES
------------------------------------------------------------------------*/

extern pval_t prmdb_prm_array[MAX_PRIME_ARRAY]; /* Extern global prime array. */
extern size_t prmdb_cnt_prm;                    /* Extern global prime count. */

/*------------------------------------------------------------------------
                            FUNCTIONS
------------------------------------------------------------------------*/

void dbfile_add_prime(pval_t num);

void dbfile_cleanup();

void dbfile_flush_primes();

int dbfile_init(const char *filename);

#endif /* _PRMDB_DBFILE_H */