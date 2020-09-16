#include <iostream>
#include <filesystem>
#include <bitcoin/bitcoin.hpp>

#include "bkidx.h"

using namespace std;

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
