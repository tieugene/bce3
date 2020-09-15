#ifndef BCE3_H
#define BCE3_H

#include <iomanip>  // string, uint*
#include <ctime>
#include "kv.h"

using namespace std;

struct  OPT_T       ///< program CLI options
{
    string      datdir;
    string      cachedir;
    uint32_t    from = 0;
    uint32_t    num = 1;
    bool        out = false;
    int         verbose = 0;
};

struct  STAT_T      ///< misc bc statistics
{
    uint32_t    blocks = 0;
    uint32_t    txs = 0;
    uint32_t    max_txs = 0;
    uint32_t    vins = 0;
    uint32_t    max_vins = 0;
    uint32_t    vouts = 0;
    uint32_t    max_vouts = 0;
    uint32_t    addrs = 0;
    uint32_t    max_addrs = 0;
    uint32_t    uniq_addrs = 0;
};

struct  COUNT_T    ///< order no counter
{
    uint32_t    bk = 0;
    uint32_t    tx = 0;
    uint32_t    addr = 0;
};

extern OPT_T    OPTS;
extern STAT_T   STAT;
extern COUNT_T  COUNT;
#ifdef MEM
extern TxMAP_T   TxDB;
extern AddrMAP_T AddrDB;
#else
extern TxDB_T   TxDB;
extern AddrDB_T AddrDB;
#endif
extern time_t   start_time;

#endif // BCE3_H
