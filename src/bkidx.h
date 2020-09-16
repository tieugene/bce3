#ifndef BKIDX_H
#define BKIDX_H



struct  FOFF_T      ///< files-offset array
  { uint32_t  fileno, offset; };

class   DATFARM_T   ///< represents blk*.dat, opening on demand
{
private:
    std::ifstream *file;
    std::string    folder;
    size_t        qty;
    bool          open(const size_t);
public:
    DATFARM_T(const size_t qty, const std::string &folder)
        : folder(folder), qty(qty)
        { file = new std::ifstream[qty]; }
    bool          read(const size_t no, const size_t offset, const int size, void *dst);
};

size_t  load_fileoffsets(const char *, FOFF_T *&);  ///< load file-offset file
bool    load_bk(DATFARM_T &, const uint32_t, const uint32_t, bc::data_chunk &);  ///< load bk to buffer

#endif // BKIDX_H
