#ifndef PRINTERS_H
#define PRINTERS_H

#include <bitcoin/bitcoin.hpp>

void    __out_bk(const bc::chain::block &);
void    __out_tx(const bc::chain::transaction &);
void    __out_vin(const bc::chain::input &);
void    __out_vout(const bc::chain::output &);
void    __out_addr(const bc::wallet::payment_address &);

void    __prn_bk(const bc::chain::block &);
void    __prn_tx(const bc::chain::transaction &);
void    __prn_vin(const bc::chain::input &);
void    __prn_vout(const bc::chain::output &);
void    __prn_addr(const bc::wallet::payment_address &);

void    __prn_head(void);
void    __prn_tail(void);
void    __prn_interim();
void    __prn_summary(void);

#endif // PRINTERS_H
