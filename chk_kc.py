#!/bin/env python3
"""
Tool to check kyotocabinet storages
"""

import sys, os
import kyotocabinet

def chk_one (dir: str, name: str) -> bool:
  """ Check one db """
  fname = os.path.join(dir, name+'.kch')
  if not os.path.isfile(fname):
    print("'{}' not exist or is not file.".format(fname))
    return False
  db = kyotocabinet.DB(kyotocabinet.DB.GEXCEPTIONAL)
  if not db.open(fname, kyotocabinet.DB.OREADER):
    print("Can't open db '{}'.".format(fname))
    return False
  print("{}: {} records".format(name, db.count()))
  return True

if __name__ == '__main__':
  if len(sys.argv) != 2:
    print("Usage: {} <k-v dir>".format(sys.argv[0]))
  else:
    dir = sys.argv[1]
    if not os.path.isdir(dir):
      print("'{}' not exist or is not folder.".format(dir))
    else:
      if chk_one(dir, 'tx'):
        chk_one(dir, 'addr')
