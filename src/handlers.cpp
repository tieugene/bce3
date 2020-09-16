#include <iostream>
#include <bitcoin/bitcoin.hpp>

#include "bce.h"
#include "misc.h"
#include "kv.h"
#include "handlers.h"
#include "printers.h"

using namespace std;

static uint32_t   BK_GLITCH[] = {91722, 91842};    // dup 91880, 91812

bool    parse_tx(const bc::chain::transaction &);
bool    parse_vin(const bc::chain::input &);
bool    parse_vout(const bc::chain::output &);
bool    parse_addr(const bc::wallet::payment_address &);

bool    parse_bk(const bc::chain::block &bk)
{
    uint32_t txs_per_bk = 0;

    if (OPTS.out) {
      if (job_mode())
        __out_bk(bk);
      else
        __prn_bk(bk);
    }
    if (!(COUNT.bk == BK_GLITCH[0] or COUNT.bk == BK_GLITCH[1])) {
        auto txs = bk.transactions();
        txs_per_bk = txs.size();
        update_stat(STAT.txs, STAT.max_txs, txs_per_bk);
        for (auto tx = txs.begin(); tx != txs.end(); tx++) {
            if (!parse_tx(*tx))
              return false;
        }
    }
    STAT.blocks++;
    return true;
}

bool    parse_tx(const bc::chain::transaction &tx)
{
    uint32_t vins_per_tx = 0, vouts_per_tx = 0;

    auto hash = tx.hash();
    if (OPTS.out) {
      if (job_mode())
        __out_tx(tx);
      else
        __prn_tx(tx);
    }
    if (job_mode()) {
      COUNT.tx = TxDB.add(hash);    // FIXME: map linker error
      if (COUNT.tx == NOT_FOUND_U32) {
        cerr << "Can't add tx " << bc::encode_hash(hash) << endl;
        return false;
      }
    }
    // 1. vins
    if (!tx.is_coinbase()) {
        auto vins = tx.inputs();
        vins_per_tx = vins.size();
        update_stat(STAT.vins, STAT.max_vins, vins_per_tx);
        for (auto vin = vins.begin(); vin != vins.end(); vin++) {
            if (!parse_vin(*vin))
              return false;
        }
    }
    // 2. vouts
    auto vouts = tx.outputs();
    vouts_per_tx = vouts.size();
    update_stat(STAT.vouts, STAT.max_vouts, vouts_per_tx);
    for (auto vout = vouts.begin(); vout != vouts.end(); vout++) {
        if (!parse_vout(*vout))
          return false;
    }
    return true;
}

bool    parse_vin(const bc::chain::input &vin)
{
    // .previous_output()
    if (OPTS.out) {
      if (job_mode())
        __out_vin(vin);
      else
        __prn_vin(vin);
    }
    return true;
}

bool    parse_vout(const bc::chain::output &vout)
{
    // .addresses()
    uint32_t addrs_per_vout = 0;
    vector<string> addr_strings;
    //bc::short_hash_list addr_hashes;

    if (OPTS.out) {
      if (job_mode())
        __out_vout(vout);
      else
        __prn_vout(vout);
    }
    auto addrs = vout.addresses();  // payment_address::list == vector<payment_address>
    addrs_per_vout = addrs.size();
    update_stat(STAT.addrs, STAT.max_addrs, addrs_per_vout);
    bc::short_hash addr_hashes[addrs_per_vout]; // sizeof=..., ~~.size()~~
    //cerr << "Addrs: sizeof " << sizeof(addr_hashes) << endl;
    size_t i = 0;
    for (auto addr = addrs.begin(); addr != addrs.end(); addr++) {
      addr_strings.push_back(addr->encoded());
      addr_hashes[i++] = addr->hash();
      parse_addr(*addr);
    }
    //return true;  // FIXME: tmp
    if (addrs_per_vout) {   // 1+ addrs
      if (addrs_per_vout == 1) {
        auto addr_no = AddrDB.get(addr_hashes[0]);
        if (addr_no == NOT_FOUND_U32) {
          addr_no = AddrDB.add(addr_hashes[0]);
          if (addr_no == NOT_FOUND_U32) {
            cerr << "Can't add addr " << addr_strings[0] << endl;
            return false;
          }
          STAT.uniq_addrs++;
        }
      } else  {
        cerr << "Sizeof " << addrs_per_vout << " Addrs is " << sizeof(addr_hashes) << endl;
        /*
        auto addr_no = AddrDB.get(addr_hashes);
        if (addr_no == NOT_FOUND_U32) {
          addr_no = AddrDB.add(addr_hashes);
          if (addr_no == NOT_FOUND_U32) {
            cerr << "Can't add addresses " << addr_strings << endl;
            return false;
          }
          STAT.uniq_addrs++;
        }*/
      }
    } else {
      // scr.to_data(true) - with prefix (script len)
      // scr.is_valid() == 1
      // scr.output_pattern() == 10 (non-standard)
      auto scr = vout.script();
      cout << "!addr:" <<
        TAB << STAT.blocks <<
        TAB << vout.value() <<
        TAB << bc::encode_base16(scr.to_data(true)) <<
        endl;
    }
    return true;
}

bool    parse_addr(const bc::wallet::payment_address &addr)
{
  // string addr.encoded(); short_hash addr.hash()
    if (OPTS.out) {
      if (job_mode())
        __out_addr(addr);
      else
        __prn_addr(addr);
    }
    return true;
}
