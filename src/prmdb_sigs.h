/* 
** NAME: sigs
**
** DESC: Cross platform signal implementation.
**/

#ifndef _PRMDB_SIGS
#define _PRMDB_SIGS

/*------------------------------------------------------------------------
                                    TYPES
------------------------------------------------------------------------*/

typedef void(*prmdb_sig_hndlr)(int);

/*------------------------------------------------------------------------
                               FUNCTIONS
------------------------------------------------------------------------*/

/* Initialize interrupt signal. */
int sigs_init(prmdb_sig_hndlr func_sigint);

#endif /* _PRMDB_SIGS */