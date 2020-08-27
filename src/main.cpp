#include <iostream>
//#include <fstream>
#include <filesystem>
#include <bitcoin/bitcoin.hpp>

#include "bce3.h"
#include "misc.h"
#include "kv.h"
#include "printers.h"
#include "handlers.h"

using namespace std;

// globals
OPT_T     OPTS;
STAT_T    STAT;
COUNT_T   COUNT;
TxDB_T    TxDB;
AddrDB_T  AddrDB;
time_t    start_time;
// locals
// consts
const uint32_t    BULK_SIZE = 1000;

struct  FOFF_T      ///< files-offset array
  { uint32_t  fileno, offset; };

class   DATFARM_T   ///< represents blk*.dat, opening on demand
{
private:
    std::ifstream *file;
    string        folder;
    size_t        qty;
    bool          open(const size_t no);
public:
    DATFARM_T(const size_t qty, const string &folder)
        : folder(folder), qty(qty)
        { file = new std::ifstream[qty]; }
    bool          read(const size_t no, const size_t offset, const int size, void *dst);
};
size_t  load_fileoffsets(const char *, FOFF_T *&);  ///< load file-offset file
bool    load_bk(DATFARM_T &, const uint32_t, const uint32_t, bc::data_chunk &);  ///< load bk to buffer

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

bool DATFARM_T::open(const size_t no) {
    if (no > qty) {
        cerr << "File # too big: " << no << endl;
        return false;
    }
    if (file[no].is_open())
        return true;
    ostringstream ss;
    ss << setw(5) << setfill('0') << no;
    string fn = folder + "blk" + ss.str() + ".dat";
    file[no].open(fn, ios::in|ios::binary);
    if (!file[no].is_open()) {
        cerr << "Can't open file " << fn << endl;
        return false;
    }
    return true;
}

bool DATFARM_T::read(const size_t no, const size_t offset, const int size, void *dst)
{
    if (!open(no))
        return false;
    file[no].seekg(offset, file[no].beg);
    file[no].read(static_cast<char *>(dst), size);
    if (file[no].gcount() != size) {
        cerr << "Can't read " << size << " bytes from " << no << "." << offset << endl;
        return false;
    }
    return true;
}

size_t  load_fileoffsets(const char *fn, FOFF_T * &foff)
{
    std::ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (!file) {            // 1. open
        cerr << "File '" << fn << "' opening failed" << endl;
        return 0;
    }
    auto data_size = file.tellg();
    if ((data_size < 0) or (data_size & 0x7) or (data_size > (8 << 20)))    // 2. chk filesize
    {
        cerr << "Wrong file size (<0 or != 8x or >8MB (1M bks)): " << fn << "=" << data_size << endl;
        return 0;
    }
    auto blocks = size_t(data_size >> 3);
    foff = new FOFF_T[blocks];
    if (!foff) {
        cerr << "Can't allocate mem for file-offset list." << endl;
        return 0;
    }
    file.seekg (0, ios::beg);
    char *tmp = static_cast<char *>(static_cast<void *>(foff));
    file.read (tmp, data_size);
    file.close();
    return blocks;
}

bool    load_bk(DATFARM_T &datfarm, const uint32_t fileno, const uint32_t offset, bc::data_chunk &BUFFER)
{
  const uint32_t    BK_SIGN = 0xD9B4BEF9;   // LE
    uint32_t sig, size;

    if (!datfarm.read(fileno, offset-8, sizeof(sig), &sig)) {
        cerr << "Can't read bk signature." << endl;
        return false;
    }
    if (sig != BK_SIGN) {
        cerr << "Block signature not found: " << hex << sig << endl;
        return false;
    }
    if (!datfarm.read(fileno, offset-4, sizeof(size), &size)) {
        cerr << "Can't read bk size." << endl;
        return false;
    }
    BUFFER.resize(size);
    if (BUFFER.size() != size) {
        cerr << "Can't resize buffer: " << size << endl;
        return false;
    }
    return datfarm.read(fileno, offset, size, &BUFFER.front());
}
