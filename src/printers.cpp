#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <bitcoin/bitcoin.hpp>

#include "bce.h"
#include "misc.h"
#include "printers.h"

using namespace std;

void    __out_bk(const bc::chain::block &bk)    ///< Output bk data for "DB"
{
    time_t t = static_cast<time_t>(bk.header().timestamp());
    cout << "b" << TAB << COUNT.bk << TAB << "'" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << "'" << endl;
}

void    __out_tx(const bc::chain::transaction &tx) {}

void    __out_vin(const bc::chain::input &vin) {}
void    __out_vout(const bc::chain::output &vout) {}
void    __out_addr(const bc::wallet::payment_address &addr) {}

void    __prn_bk(const bc::chain::block &bk)
{
    cout <<
        "Bk:" <<
        TAB << COUNT.bk <<
        TAB << bk.header().timestamp() <<
    endl;
    // << "\thash:" << bc::encode_hash(bk.hash())
}

void    __prn_tx(const bc::chain::transaction &tx)
{
    cout <<
        TAB << "Tx:" <<
        TAB << bc::encode_hash(tx.hash()) <<
    endl;
}

void    __prn_vin(const bc::chain::input &vin)
{
    // TODO: txid (.previous_output()?), no (~~.sequence()~~)
    cout <<
        TAB << TAB << "<=:" <<
        TAB << "[# = ..., № = ...]" <<
    endl;
}

void    __prn_vout(const bc::chain::output &vout)
{
    // TODO: no (!)
    cout <<
        TAB << TAB << "=>:" <<
        TAB << vout.value() <<
    endl;
}

void    __prn_addr(const bc::wallet::payment_address &addr)
{
    // Note: '<< addr' == addr.encoded()
    cout << TAB << TAB << TAB <<
        "Addr:" << TAB << addr.encoded() << TAB << bc::encode_base16(addr.hash()) <<
    endl;
}

void    __prn_tail(void)
{
  cerr << "---\t-------\t-------\t-------\t-------\t-------\t-----" << endl;
}

void    __prn_head(void)
{
  cerr << "Bk\tTx\tVins\tVouts\tAddrs\tUAddrs\tTime" << endl;
  __prn_tail();
}

void    __prn_interim() {
    cerr <<
         (COUNT.bk+1)/1000 <<
         TAB << STAT.txs <<
         TAB << STAT.vins <<
         TAB << STAT.vouts <<
         TAB << STAT.addrs <<
         TAB << STAT.uniq_addrs <<
         TAB << time(nullptr) - start_time <<
         TAB << memused() <<
    endl;
}


void    __prn_summary(void)
{
    cerr << "= Summary =" << endl
        << "Time:" << TAB << time(nullptr) - start_time << endl
        << "Blocks:" << TAB << STAT.blocks << endl;
    //if (OPTS.verbose)   // >2
        cerr
            << "Tx:" << TAB << STAT.txs << endl
            << "Vins:" << TAB << STAT.vins << endl
            << "Vouts:" << TAB << STAT.vouts << endl
            << "Addrs:" << TAB << STAT.addrs << endl
            << "Uniq Addrs:" << TAB << STAT.uniq_addrs << endl
            << "Max tx/bk:" << TAB << STAT.max_txs << endl
            << "Max vins/tx:" << TAB << STAT.max_vins << endl
            << "Max vouts/tx:" << TAB << STAT.max_vouts << endl
            << "Max addrs/vout:" << TAB << STAT.max_addrs << endl;
}
