#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>


//inialization for slot set and cache implementation
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

  //intialize writeAlloc writeBack and lru
  bool writeAlloc, writeBack, lru;
  std::string wa = argv[4];
  std::string wt = argv[5];
  std::string ev = argv[6];

  //makesure our parameters are valid or invalid
  //handle cases where our program is given bad input
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

  //write alloc
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

  //evictions
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

  //if writeallocate is false and writeback is true
  if (!writeAlloc && writeBack) {
    std::cerr << "write should allocate and also write back" << std::endl;
    return 1;
  }

  //offset and index initalization
  uint32_t offsetBits = bitSize(blkSize);
  uint32_t indexBits = bitSize(numSets);

  //initalize our cache simulation
  Cache newCache;
  newCache.sets.resize(numSets);
  for (auto &set : newCache.sets) {
    set.slots.resize(numBlocks);
  }

  //loads stores, etc...
  uint32_t totalLoads = 0;
  uint32_t totalStores = 0;
  uint32_t loadHits = 0;
  uint32_t loadMisses = 0;
  uint32_t storeHits = 0;
  uint32_t storeMisses = 0;
  uint32_t totalSizes = 0;
  uint32_t timestamps = 0;
  

  //parse the traces
  std::string line;
  while(std::getline(std::cin, line)) {
    if(!line.empty()) {

      char operation;
      uint32_t address;
      int ignore;

      //string stream to help parse the input
      std::stringstream ss(line);
      ss >> operation;
      ss >> std::hex >> address;
      ss >> std::dec >> ignore;
  
      // Extract index and tag 
      uint32_t index = (address >> offsetBits) & ((1u << indexBits) - 1);
      uint32_t tag = address >> (offsetBits + indexBits);
  
      Set &set = newCache.sets[index];
      timestamps++;
  
      //update load or store values
      bool isLoad = (operation == 'l');
      if(isLoad) {
        totalLoads++;
      } else {
        totalStores++;
      }
    }
  }


  return 0;
}

//size helper function
uint32_t bitSize(uint32_t n) {
  uint32_t size = 0;
  while (n > 1) {
    n = n >> 1;
    size++;
  }
  return size;
}




