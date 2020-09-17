#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/resource.h>

#include "bce.h"
#include "misc.h"

using namespace std;

const string TAB = "\t";
static string  help_txt = "\
Usage: [options] <file-offset_file>\n\
Options:\n\
-f n      - block starts from (default=0)\n\
-n n      - blocks to process (default=1, 0=all)\n\
-d <path> - *.dat folder (default='' - current folder)\n\
-c <path> - cache data folder\n\
-o        - out results\n\
-v[n]     - verbose (debug info to stderr):\n\
    0 - errors only (default)\n\
    1 - short info (default n)\n\
    2 - mid\n\
    3 - full debug\n\
";

void    __prn_opts(void)
{
    cerr
        << "Options:" << endl
        << TAB << "From:" << TAB << OPTS.from << endl
        << TAB << "Num:" << TAB << OPTS.num << endl
        << TAB << "Out:" << TAB << OPTS.out << endl
        << TAB << "Debug:" << TAB << OPTS.verbose << endl
        << TAB << "DatDir:" << TAB << OPTS.datdir << endl
        << TAB << "Cache:" << TAB << OPTS.cachedir << endl
    ;
}

bool    cli(int argc, char *argv[])
{
    int opt;
    bool retvalue = false;

    OPTS.from = 0;
    OPTS.num = 1;
    OPTS.datdir = "";
    //OPTS.cachedir = ".";
    OPTS.out = false;
    OPTS.verbose = 0;
    while ((opt = getopt(argc, argv, "f:n:d:c:ov::")) != -1)
    {
        switch (opt) {
            case 'f':   // FIXME: optarg < 0 | > 999999
                OPTS.from = atoi(optarg);
                break;
            case 'n':   // FIXME: optarg < 1 | > 999999
                //OPTS.num = *optarg == '*' ? 999999 : atoi(optarg);
                OPTS.num = atoi(optarg);
                if (OPTS.num == 0)
                    OPTS.num = 999999;
                break;
            case 'd':
                OPTS.datdir = optarg;
                break;
            case 'c':
                OPTS.cachedir = optarg;
                break;
            case 'o':
                OPTS.out = true;
                break;
            case 'v':   // FIXME: optarg = 0..5
                OPTS.verbose = (optarg) ? atoi(optarg) : 1;
                break;
            case '?':   // can handle optopt
                cerr << help_txt << endl;
                break;
        }
    }
    // opterr - allways 1
    // optind - 1st file argv's no (if argc > optind)
    if (optind == (argc-1))  {
        retvalue = true;
        if (OPTS.verbose > 1)   // TODO: up v-level
            __prn_opts();
    } else
        cerr << "Error: file-offset filename required." << endl << help_txt;
    return retvalue;
}

void    update_stat(uint32_t &counter, uint32_t &maxer, const uint32_t v) {
    counter += v;
    if (maxer < v)
        maxer = v;
}

long memused(void) {
    rusage rused;
    long retvalue = 0;
    if (getrusage(RUSAGE_SELF, &rused) == 0) {
      retvalue = rused.ru_maxrss;
    }
    return retvalue;
}
