#include <signal.h>
#include "prmdb_sigs.h"
#include "prmdb_types.h"

/*------------------------------------------------------------------------
                              FUNCTIONS
------------------------------------------------------------------------*/

int sigs_init(prmdb_sig_hndlr func_sigint)
{
    prmdb_sig_hndlr sig_rc;
    int rc;

    rc = PRMDB_OK;
    //s_func_sigint = func_sigint;

    sig_rc = signal(SIGINT, func_sigint);
    if (sig_rc == SIG_ERR)
    {
        rc = PRMDB_ERROR;
    }

    //rc = plat_specific_init();

    //return rc;
    return PRMDB_OK;
}