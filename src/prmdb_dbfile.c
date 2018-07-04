#include <stdio.h>
#include <string.h>

#include "prmdb_assert.h"
#include "prmdb_dbfile.h"
#include "prmdb_log.h"

/*------------------------------------------------------------------------
                            EXTERN VARIABLES
------------------------------------------------------------------------*/


pval_t prmdb_prm_array[MAX_PRIME_ARRAY];    /* Extern global prime array. */
size_t prmdb_cnt_prm;                       /* Extern global prime count. */

/*------------------------------------------------------------------------
                            STATIC VARIABLES
------------------------------------------------------------------------*/

/* File related: */
static FILE     *s_fp;              /* Prime db file handle. */
static size_t   s_next_write_idx;   /* Index to start writing on flush. */

/*------------------------------------------------------------------------
                               FUNCTIONS
------------------------------------------------------------------------*/

void dbfile_add_prime(pval_t num)
{
    PRMDB_ASSERT_FATAL(prmdb_cnt_prm < MAX_PRIME_ARRAY, "Prime array maximum bounds overflow.");

    prmdb_prm_array[prmdb_cnt_prm] = num;
    ++prmdb_cnt_prm;
}

void dbfile_cleanup()
{
    int close_res;

    /* Close database handle. */
    if (s_fp != NULL)
    {
        close_res = fclose(s_fp);
        if (close_res != 0)
        {
            log_info("Failed to close file handle.");
        }
    }
}

void dbfile_flush_primes()
{
    int         flush_res;
    pval_t      i;
    size_t      num_written;
    uint32_t    write_val;

    PRMDB_ASSERT_FATAL(s_fp != NULL, "File handle is NULL.");

    /*
    ** If there are no values in the db file, then start at index 0.
    ** Otherweise, start at the number after the last written index.
    */
    i = s_next_write_idx;

    num_written = 0;

    for (i; i < prmdb_cnt_prm; ++i)
    {
        write_val = prmdb_prm_array[i];
        fwrite(&write_val, sizeof(write_val), 1, s_fp);
        ++num_written;
    }

    /*
    ** Note: This assert is DEBUG only becuase the case of a SIGINT may not have time
    ** to compute any primes.
    */
    PRMDB_ASSERT_DEBUG(num_written > 0, "No primes written to disk.");

    /* Flush the file. If the file didn't flush properly, error. */
    flush_res = fflush(s_fp);
    PRMDB_ASSERT_FATAL(flush_res != EOF, "Failed to flush file.");

    s_next_write_idx = prmdb_cnt_prm;
    log_info("Wrote %u values to db file.", num_written);

}

int dbfile_init(const char *filename)
{
    uint32_t    in_num;
    int         rc;
    size_t      read_ind;

    s_next_write_idx = 0;

    /* Static memory */
    log_debug("Initializing static memory...");
    memset(prmdb_prm_array, 0, sizeof(pval_t) * MAX_PRIME_ARRAY);
    prmdb_cnt_prm = 0;

    /* Try to open an existing file. */
    s_fp = fopen(filename, "r+b");
    if (s_fp != NULL)
    {
        read_ind = fread(&in_num, sizeof(in_num), 1, s_fp);
        while (read_ind > 0)
        {
            dbfile_add_prime(in_num);
            read_ind = fread(&in_num, sizeof(in_num), 1, s_fp);
        }

        /* Set the last written index to the last prime number read. */
        s_next_write_idx = prmdb_cnt_prm;

        if (prmdb_cnt_prm == 0)
        {
            log_fatal("Read zero primes from prime db file. Delete file and rereun.");
            rc = PRMDB_ERROR;
        }
        else
        {
            log_info("Read %u primes from \"%s\".", prmdb_cnt_prm, filename);
            rc = PRMDB_OK;
        }
    }
    else
    {
        /* Create a new file and init a few values. */
        log_info("No existing file. Creating new...");

        s_fp = fopen(filename, "w+b");

        if (s_fp == NULL)
        {
            log_fatal("Failed to open file \"%s\".", filename);
            rc = PRMDB_FILE_ERROR;
        }
        else
        {
            /* Add a few known prime values. */
            dbfile_add_prime(3);
            dbfile_add_prime(5);
            dbfile_add_prime(7);
            dbfile_add_prime(11);

            /* Flush the few new values to file. */
            dbfile_flush_primes();
            rc = PRMDB_OK;
        }
    }

    return rc;
}