#include "kv.h"

bool      KVDB_T::init(string &s) {
  counter = 0;
  auto retvalue = db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE); //  | kyotocabinet::PolyDB::OTRUNCATE
  if (retvalue) {
    auto i = db.count();
    if (i >= 0)
      counter = i;
    else
      retvalue = false;
  }
  return retvalue;
}

void      KVDB_T::clear(void) {
  db.clear();
  counter = 0;
}

uint32_t  KVDB_T::count(void) {
  return counter;
/*  auto retvalue = db.count();
  if (retvalue < 0)
      return NOT_FOUND_U32;
  return retvalue;  // was uint32_t(retvalue)*/
}

uint32_t  KVDB_T::__add(void *k_ptr, size_t size) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = NOT_FOUND_U32;
    void *v_ptr = static_cast<void *>(&counter);
    if (db.add(static_cast<char *>(k_ptr), size, static_cast<char *>(v_ptr), sizeof(uint32_t))) {
        value = counter;
        counter++;
    }
    return value;
}

uint32_t  KVDB_T::__get(void *k_ptr, size_t size) {
    auto value = NOT_FOUND_U32;
    void *v_ptr = static_cast<void *>(&value);
    auto result = db.get(static_cast<char *>(k_ptr), size, static_cast<char *>(v_ptr), sizeof(uint32_t));
    // ???
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t  TxDB_T::add(bc::hash_digest &key) {
  return __add(static_cast<void *>(&key), sizeof(bc::hash_digest));
}

uint32_t    TxDB_T::get(bc::hash_digest &key) {
  return __get(static_cast<void *>(&key), sizeof(bc::hash_digest));
}

uint32_t    AddrDB_T::add(bc::short_hash &key) {
    //auto result = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = NOT_FOUND_U32;
    void *k_ptr = static_cast<void *>(&key);
    void *v_ptr = static_cast<void *>(&value);
    if (db.add(static_cast<char *>(k_ptr), sizeof(bc::short_hash), static_cast<char *>(v_ptr), sizeof(uint32_t))) {
        value = counter;
        counter++;
    }
    return value;
}

uint32_t    AddrDB_T::get(bc::short_hash &key) {
    uint32_t value = NOT_FOUND_U32;
    void *k_ptr = static_cast<void *>(&key);
    void *v_ptr = static_cast<void *>(&value);
    auto result = db.get(static_cast<char *>(k_ptr), sizeof(bc::short_hash), static_cast<char *>(v_ptr), sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    AddrDB_T::add(bc::short_hash_list &key) {
  return __add(static_cast<void *>(&key), sizeof(bc::hash_digest));
}

uint32_t    AddrDB_T::get(bc::short_hash_list &key) {
  return __get(static_cast<void *>(&key), sizeof(bc::hash_digest));
}
