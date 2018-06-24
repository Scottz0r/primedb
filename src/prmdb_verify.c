#include <stdio.h>
#include "prmdb_assert.h"
#include "prmdb_log.h"
#include "prmdb_verify.h"

#define VFY_MSG_INTERVAL 100000

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
            log_fatal("Number \"%u\" at ps %u is not prime!", in_num, counter);
            result = false;
            break;
        }

        ++counter;
        if (counter % VFY_MSG_INTERVAL == 0)
        {
            log_info("Verified %u primes.", counter);
        }


        read_ind = fread(&in_num, sizeof(in_num), 1, fp);
    }

    fclose(fp);

    PRMDB_ASSERT_FATAL(counter != 0, "No primes found in file to check!");

    return result;
}


static bool check_prime(pval_t val)
{
    pval_t  half_val;
    size_t  i;
    bool    result;

    if (val % 2 == 0)
    {
        /* If even, not prime. */
        result = false;
    }
    else if (val <= 11)
    {
        /* Manual check for small values. */
        if ((val == 3) || (val == 5) || (val == 7) || (val == 11))
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        /* Loop odd numbers while less than half of the value. */
        half_val = (val / 2) + 1;

        for (i = 3; i < half_val; i += 2)
        {
            if (val % i == 0)
            {
                result = false;
                break;
            }
        }

        result = true;
    }

    return result;
}