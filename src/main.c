/*

TODO List:
    [ ] Makefile and build in Cygwin?

    [ ] Verification module. Checks every number in DB for validity.
    [ ] Different flag for processing and verifying?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "prmdb_assert.h"
#include "prmdb_dbfile.h"
#include "prmdb_log.h"
#include "prmdb_sigs.h"
#include "prmdb_types.h"
#include "prmdb_verify.h"

/*------------------------------------------------------------------------
                                LITERALS
------------------------------------------------------------------------*/

#define PRIME_SEARCH_MAX    0xFFFFFFFF  /* Stopping point for searching primes. */

#define MSG_INTERVAL    100000  /* Check message interval. */
#define FLS_INTERVAL    100000  /* Interval to flush numbers to file. */

/* Additional exit codes. */
#define PRMDB_EXIT_ARGS       2
#define PRMDB_EXIT_SIGINT   128

/* Log level currently set by configuration. Probably should be based on config file. */
#ifdef _DEBUG
#define BLD_LOG_LEVEL PRMDB_LOG_DEBUG
#else
#define BLD_LOG_LEVEL PRMDB_LOG_INFO
#endif

/*------------------------------------------------------------------------
                                  TYPES
------------------------------------------------------------------------*/

typedef enum prgm_run_type
{
    PRMDB_FIND,
    PRMDB_VERIFY
} prgm_run_type;

typedef struct prgm_args
{
    const char      *db_filename;
    prgm_run_type   run_type;
} prgm_args;

/*------------------------------------------------------------------------
                              STATIC VARIABLES
------------------------------------------------------------------------*/

static size_t       s_start_count;  /* Start count of prime numbers */
static clock_t      s_start_clock;  /* Start sys clock of processing */
static prgm_args    s_prgm_args;    /* Command line arguments */

/*------------------------------------------------------------------------
                            FUNCTION DECLARATIONS
------------------------------------------------------------------------*/

static void check_file();

static void find_primes();

static bool is_prime(pval_t num);

static void init(int argc, const char **argv);

static void cb_on_exit(void);

static void on_sigint(int param);

static bool parse_args(int argc, const char **argv, prgm_args *out_args);

static void print_stats();

/*------------------------------------------------------------------------
                               FUNCTIONS
------------------------------------------------------------------------*/

void main(int argc, const char *argv[])
{
    init(argc, argv);

    switch (s_prgm_args.run_type)
    {
    case PRMDB_FIND:
        find_primes();
        break;
    case PRMDB_VERIFY:
        check_file();
        break;
    default:
        log_fatal("Unknown run type.");
        exit(EXIT_FAILURE);
        break;
    }

    exit(EXIT_SUCCESS);
}

static void cb_on_exit(void)
{
    log_info("Shutting down program...");

    /* Call cleanup functions of modules. These shoudl never error. */
    dbfile_cleanup();
    log_cleanup();
}

static void check_file()
{
    bool    result;

    /* 
    ** Call verify module to check db file. If check result is false, then return 
    ** failure code. 
    */
    result = verify_chk_file(s_prgm_args.db_filename);

    if (result == false)
    {
        exit(EXIT_FAILURE);
    }
}

static void find_primes()
{
    pval_t  counter;
    pval_t  i;
    int     i_rc;
    bool    is_prm;

    /* Initialize DB file*/
    i_rc = dbfile_init(s_prgm_args.db_filename);
    PRMDB_ASSERT_FATAL(i_rc == PRMDB_OK, "DB File initialization failed.");

    /* Assert data initialized correctly. */
    PRMDB_ASSERT_FATAL(prmdb_cnt_prm > 0, "Prime array empty or count not set.");

    counter = 0;

    /* Initialize stat tracking static variables. */
    s_start_clock = clock();
    s_start_count = prmdb_cnt_prm;

    /* Start searching at the last loaded prime + 2. */
    i = prmdb_prm_array[prmdb_cnt_prm - 1] + 2;

    log_info("Starting calculating from %u", i);

    /* Loop until maximum search number hit or prime array full. */
    for (; (i < PRIME_SEARCH_MAX) && (prmdb_cnt_prm < MAX_PRIME_ARRAY); i += 2)
    {
        is_prm = is_prime(i);
        if (is_prm == true)
        {
            dbfile_add_prime(i);
        }

        ++counter;
        if ((counter % MSG_INTERVAL) == 0)
        {
            log_info("Checked %u (%u primes)", counter, prmdb_cnt_prm);
        }

        if ((counter % FLS_INTERVAL) == 0)
        {
            dbfile_flush_primes();
        }
    }

    /* Print stats if processing completes. */
    print_stats();
}

static void init(int argc, const char **argv)
{
    bool    arg_res;
    int     i_rc;

    /* Exit callback */
    atexit(cb_on_exit);

    /* Log */
    log_init(BLD_LOG_LEVEL);
    log_info("Logging start.");

    /* Program arguments. */
    arg_res = parse_args(argc, argv, &s_prgm_args);
    if (arg_res != true)
    {
        /* Log message and terminate program. */
        log_fatal("Invalid program arguments. Usage: prmdb [find|verify] [db_filename]");
        exit(PRMDB_EXIT_ARGS);
    }

    /* Signals */
    log_debug("Initializing signals...");
    i_rc = sigs_init(on_sigint);
    PRMDB_ASSERT_FATAL(i_rc == PRMDB_OK, "Signal initialization failed.");
}

static bool is_prime(pval_t num)
{
    PRMDB_ASSERT_FATAL(prmdb_cnt_prm > 0, "Prime count zero.");
    PRMDB_ASSERT_DEBUG((num % 2) != 0, "Test number is even.");

    bool    result;
    pval_t  *ptr_end;
    pval_t  *ptr_prime;

    result = true;
    ptr_prime = prmdb_prm_array;
    ptr_end = prmdb_prm_array + prmdb_cnt_prm;

    /*
    ** Iterate through previously found primes, checking if remainder with current number 
    ** is zero. 
    */
    for (; ptr_prime < ptr_end; ++ptr_prime)
    {
        if (num % *ptr_prime == 0)
        {
            result = false;
            break;
        }
    }

    return result;
}

static void on_sigint(int param)
{
    /*
    ** Do a somewhat graceful exit on a SIGINT. Flush any computed, but not written, primes
    ** and exit program after closing handles.
    */
    log_info("SIGINT recieved.");

    /* If finding primes, flush any un-saved values and show stats. */ 
    if (s_prgm_args.run_type == PRMDB_FIND)
    {
        dbfile_flush_primes();
        print_stats();
    }

    exit(PRMDB_EXIT_ARGS);
}

static bool parse_args(int argc, const char **argv, prgm_args *out_args)
{
    bool    result;

    result = true;
    
    if (out_args == NULL)
    {
        result = false;
    }
    else if (argc < 3)
    {
        result = false;
    }
    else
    {
        out_args->db_filename = argv[2];
        
        if (strcmp(argv[1], "find") == 0)
        {
            out_args->run_type = PRMDB_FIND;
        }
        else if (strcmp(argv[1], "verify") == 0)
        {
            out_args->run_type = PRMDB_VERIFY;
        }
        else
        {
            result = false;
        }
    }

    return result;
}

static void print_stats()
{
    clock_t     end_clock;
    size_t      total_calc;

    double      exec_time;
    int         hr;
    int         min;
    int         sec;

    /* Display number computed and time taken. */
    end_clock = clock();
    exec_time = (double)(end_clock - s_start_clock) / (double)CLOCKS_PER_SEC;

    hr = (int)exec_time / 3600;
    min = ((int)exec_time % 3600) / 60;
    sec = ((int)exec_time % 3600) % 60;

    total_calc = prmdb_cnt_prm - s_start_count;

    log_info("Calculated %u primes in %02d:%02d:%02d", total_calc, hr, min, sec);
}
