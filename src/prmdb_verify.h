/*
** NAME: verify
**
** DESC: Verify a prime db file is valid.
**/
#ifndef PRMDB_VERIFY_H
#define PRMDB_VERIFY_H

#include "prmdb_types.h"

bool verify_chk_file(const char *filename);

#endif /* PRMDB_VERIFY_H */