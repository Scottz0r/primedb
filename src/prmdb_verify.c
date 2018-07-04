#include <stdio.h>
#include "prmdb_assert.h"
#include "prmdb_log.h"
#include "prmdb_verify.h"

#define VFY_MSG_INTERVAL 1000000

static bool check_prime(pval_t val);


bool verify_chk_file(const char *filename)
{
    pval_t      counter;
    FILE        *fp;
    size_t      fsize;
    uint32_t    in_num;
    bool        is_prm;
    size_t      read_ind;
    bool        result;

    counter = 0;
    result = true;

    fp = fopen(filename, "r+b");
    PRMDB_ASSERT_FATAL(fp != NULL, "Failed to open prime db file.");

    /* Display start message with some numbers. */
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    log_info("Verifying \"%s\" (%u bytes, %u primes)", filename, fsize, (fsize / sizeof(in_num)));

    /* Loop through file contents, checking values. */
    read_ind = fread(&in_num, sizeof(in_num), 1, fp);
    while (read_ind > 0)
    {
        is_prm = check_prime(in_num);

        if (is_prm == false)
        {
            log_fatal("Number \"%u\" at index %u is not prime!", in_num, counter);
            result = false;
        }

        ++counter;
        if (counter % VFY_MSG_INTERVAL == 0)
        {
            log_info("Checked %u primes.", counter);
        }


        read_ind = fread(&in_num, sizeof(in_num), 1, fp);
    }

    log_info("Checked %u primes with result %d.", counter, result);

    fclose(fp);

    PRMDB_ASSERT_FATAL(counter != 0, "No primes found in file to check!");

    return result;
}


static bool check_prime(pval_t val)
{
    uint_fast64_t   i;
    uint_fast64_t   i_sqrd;
    bool            result;

    if ((val % 2 == 0) || (val % 3 == 0))
    {
        result = true;
    }
    else
    {
        /*
        ** Use "Square Root" method:
        ** 
        ** All primes can be expressed as 6k +/- 1 (except 2 and 3).
        **
        ** Divisors repeat themselves at sqrt(k)
        **
        ** So, check 6k +/- 1 <= sqrt(n)
        */

        result = true;
        i = 5;
        i_sqrd = i * i;

        while ((i_sqrd <= val) && (result == true))
        {
            if ((val % i == 0) || (val % (i + 2) == 0))
            {
                result = false;
            }

            i += 6;
            i_sqrd = i * i;
        }
    }

    return result;
}