// Tool to check kyotocabinet db
// Build: g++ -o chk_kc -lkyotocabinet chk_kc.cpp

#include <ostream>
#include <kcpolydb.h>

using namespace std;

bool chk_one(const string dir, const string name) {
  kyotocabinet::PolyDB db;

  auto path = dir + "/" + name + ".kch";
  if (!db.open(path, kyotocabinet::PolyDB::OREADER)) {
    cerr << "Can't open db '" << path << "'." << endl;
    return false;
  }
  cout << name << ": " << db.count() << endl;
  return true;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <k-v dir>" << endl;
    return 1;
  }
  if (chk_one(argv[1], "tx"))
    chk_one(argv[1], "addr");
  return 0;
}
