#ifndef KV_H
#define KV_H

#include <bitcoin/bitcoin.hpp>
#include <kcpolydb.h>
#include <array>    // FIXME:

using namespace std;
//using namespace kyotocabinet;

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

class   KVDB_T {
protected:
  uint32_t  counter;
  kyotocabinet::PolyDB     db;
  uint32_t    __add(void *, size_t);
  uint32_t    __get(void *, size_t);
public:
  KVDB_T      (void) : counter(0) {}
  bool        init(string &);
  void        clear();
  uint32_t    count(void);
};

class   TxDB_T : public KVDB_T {
// hash_digest = ... bytes
public:
  uint32_t    add(bc::hash_digest &);
  uint32_t    get(bc::hash_digest &);
};

class   AddrDB_T : public KVDB_T {
// short_hash = 20 bytes
public:
  uint32_t    add(bc::short_hash &);
  uint32_t    get(bc::short_hash &);
  uint32_t    add(bc::short_hash_list &);
  uint32_t    get(bc::short_hash_list &);
};

#endif // KV_H
