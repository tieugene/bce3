# bce3
BitCoin Explorer v.3

## TODO
- kv:
  - save counter
  - get_update
- modes (-c/-o options):
  - count + info
  - k-v + DB out
- start/continue (-c)
- multiaddress: пропускает
  - bk: 164467
  - tx: 15 (60a20bd93aa49ab4b28d514ec10b06e1829ce6818ec06cd3aabd013ebcdc4bb1)
  - vout: 0.01 => 1AJbsFZ64EpEfS5UAjAfcUG8pH8Jn3rn1F, 1A8JiWcwvpY7tAopUkSnGuEYHmzGYfZPiq
- KV(byte_array)
- find:
  - vin.# (test serialization to info/json)
  - vin.№
  - vout.№ [=counter]
- move to git
- lint
- info: verbisity - bc[,tx[,vin,vout[addr]]]

## Done
- log to file: `> out.log 2> >(tee err.log >&2)`
- clean:
  - static check (cppcheck)
  - const

## Try
- test drive: bonnie++
- USB flash
- RAMdisk (/tmp)
- KV as stl::unurdered_map (RAM only)
- static

## Notes:
- encoded() оборачивает _hash, как и hash() => можно вызывать оба
- bitcoin::hash::math.hash
- typedef byte_array<20> short_hash == struct (w/ .size())
- typedef vector<short_hash> short_hash_list;
- vector<byte_array>:
  - size() == qty of elements
  - byte_array* data() ==  direct access
### empty addrs:
- 71036 (x 3), 'non-standard', extra '0xAC' after end

## test
- 194k: 693", 1.2GB RAM, EOF
