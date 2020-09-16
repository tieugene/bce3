#include <iostream>
//#include <fstream>
#include <filesystem>
#include <bitcoin/bitcoin.hpp>

#include "bce.h"
#include "bkidx.h"
#include "misc.h"
#include "kv.h"
#include "printers.h"
#include "handlers.h"

using namespace std;

// globals
OPT_T     OPTS;
STAT_T    STAT;
COUNT_T   COUNT;
#ifdef MEM
TxMAP_T    TxDB;
AddrMAP_T  AddrDB;
#else
TxDB_T    TxDB;
AddrDB_T  AddrDB;
#endif
time_t    start_time;
// locals
// consts
const uint32_t    BULK_SIZE = 10000;

int main(int argc, char *argv[])
{
    bc::chain::block bk;
    FOFF_T*    FOFF;
    bc::data_chunk BUFFER;

    // 1. handle CLI
    if (!cli(argc, argv))  // no file defined
        return 1;
    auto bk_no_upto = OPTS.from + OPTS.num;
    // 1.1. prepare bk info
    auto bk_qty = load_fileoffsets(argv[argc-1], FOFF);
    if (!bk_qty)
        return 1;
    if (bk_qty < bk_no_upto) {
        cerr << "Loaded blocks (" << bk_qty << ") < max wanted " << bk_no_upto << endl;
        return 1;
    }
    // 1.2. prepare blk######.dat loader
    if (!OPTS.datdir.empty() and OPTS.datdir.back() != '/')
        OPTS.datdir += '/';  // FIXME: native OS path separator
    DATFARM_T datfarm(bk_qty, OPTS.datdir);
    // 1.3. prepare k-v storages
    if (job_mode()) {
      if (OPTS.cachedir.back() != '/')
          OPTS.cachedir += '/';  // FIXME: native path separator
      auto s = OPTS.cachedir + "tx.kch";
      if (!TxDB.init(s)) {
          cerr << "Can't open 'tx' cache: " << s << endl;
          return 1;
      }
      s = OPTS.cachedir + "addr.kch";
      if (!AddrDB.init(s)) {
          cerr << "Can't open 'addr' cache " << s << endl;
          return 1;
      }
      if (OPTS.from == 0) {
        TxDB.clear();
        AddrDB.clear();
      }
    }
    // 1.4. last prestart
    start_time = time(nullptr);
    if (OPTS.verbose)
      __prn_head();
    // 2. main loop
    for (COUNT.bk = OPTS.from; COUNT.bk < bk_no_upto; COUNT.bk++)
    {
        if (!load_bk(datfarm, FOFF[COUNT.bk].fileno, FOFF[COUNT.bk].offset, BUFFER))
            break;
        if (!bk.from_data(BUFFER)) {
            cerr << "Bad chunk." << endl;
            break;
        }
        if (!parse_bk(bk))
            break;
        if ((OPTS.verbose) and (((COUNT.bk+1) % BULK_SIZE) == 0))
            __prn_interim();
    }
    if (OPTS.verbose) {
      COUNT.bk--;
      __prn_tail();
      __prn_interim();
      if (OPTS.verbose >= 2)
        __prn_summary();
    }
    return 0;
}
