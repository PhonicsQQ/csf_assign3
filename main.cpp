#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>

struct Slot {
  uint32_t tag;
  bool valid, dirty;
  uint32_t load_ts, access_ts;
};

struct Set {
  std::vector<Slot> slots;
};

struct Cache {
  std::vector<Set> sets;
};

bool isPowderOf2(uint64_t n) {
  return n && !(n & (n - 1));
}

int main( int argc, char **argv ) {
  //handle incorrect amount
  if (argc != 7) {
    std::cerr << "Expected 6 elements but got " << (argc - 1) << std::endl;
    return 1;
  }

  //arguments from binary to decimal
  uint32_t numSets = atoi(argv[1]);
  uint32_t numBlocks = atoi(argv[2]);
  uint32_t blkSize = atoi(argv[3]);
  if (!isPowderOf2(numSets) || !isPowderOf2(numBlocks) || !isPowderOf2(blkSize)) {
    std::cerr << "All arguments must be powers of 2." << std::endl;
    return 1;
  }

  if (blkSize < 4) {
    std::cerr << "Block size must be greater than 4 "<< std::endl;
    return 1;
  }

  bool writeAlloc, writeBack, lru;
  std::string wa = argv[4];
  std::string wt = argv[5];
  std::string ev = argv[6];
  if (wa == "write-allocate") {
    writeAlloc = true;
  }
  else if(wa == "no-write-allocate") {
    writeAlloc = false;
  }
  else {
    std::cerr << "invalid write-allocate arg" << std::endl;
    return 1; 
  }

  if (wt == "write-through") {
    writeBack = false;
  }
  else if(wt == "write-back") {
    writeBack = true;
  }
  else {
    std::cerr << "invalid write-through arg" << std::endl;
    return 1; 
  }

  if (ev == "lru") {
    lru = true;
  }
  else if(ev == "fifo") {
    lru = false;
  }
  else {
    std::cerr << "invalid eviction arg" << std::endl;
    return 1; 
  }
  
  if (!writeAlloc && writeBack) {
    std::cerr << "write should allocate and also write back" << std::endl;
    return 1;
  }

  return 0;
}


