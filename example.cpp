// An example of libbitcoin usage
// build: gcc-c++ -lbitcoin -o example example.cpp

#include <bitcoin/bitcoin.hpp>

using namespace std;
using namespace bc;

int main(int argc, char *argv[])
{
    const string bk0_hex = "0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a29ab5f49ffff001d1dac2b7c0101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff4d04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000";
    data_chunk bk_bin;
    chain::block bk;
    cout << "Hello World!" << endl;
    if (!decode_base16(bk_bin, bk0_hex)) {
        cerr << "Bad hex." << endl;
        return 1;
    }
    if (!bk.from_data(bk_bin)) {
        cerr << "Bad chunk." << endl;
        return 1;
    }
    cout << "Head:" << endl;
    cout << "hash: " << encode_hash(bk.hash()) << endl;    // == bk.header().hash()
    cout << "time: " << bk.header().timestamp() << endl;
    auto txs = bk.transactions();
    for (auto tx = txs.begin(); tx != txs.end(); tx++) {
        cout << encode_hash(tx->hash()) << endl;
    }
    cout << "Total inputs:" << bk.total_inputs() << endl;
    return 0;
}
