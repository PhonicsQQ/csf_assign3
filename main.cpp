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

// CacheData struct to track number of loads, stores, hits, misses, etc. in cache
struct CacheData {
  uint64_t totalLoads = 0;
  uint64_t totalStores = 0;
  uint64_t loadHits = 0;
  uint64_t loadMisses = 0;
  uint64_t storeHits = 0;
  uint64_t storeMisses = 0;
  uint64_t totalCycles = 0;
  uint64_t timestamps = 0;
};

// Flags struct to track user input flags for writeAlloc, writeBack, and lru
struct Flag {
    bool writeAlloc;
    bool writeBack;
    bool lru;
};

// returns whether uint64_t n is a power of 2
bool isPowerOf2(uint64_t n) {
  return n && !(n & (n - 1));
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

//empty slot helper
int findEmpty(const Set &set) {
  for (int i = 0; i < (int)set.slots.size(); i++) {
    if (!set.slots[i].valid) {
      return i;
    }
  }
  return -1;
}

//find oldest slot using either methodology
int findEvict(const Set &set, bool lru) {
  uint32_t evictIndex = 0;
  uint32_t lastIn = lru ? set.slots[0].access_ts : set.slots[0].load_ts;
  uint32_t currIn;
  for (int i = 0; i < (int)set.slots.size(); i++) {
    if(lru) {
      currIn = set.slots[i].access_ts;

    }
    else {
      currIn = set.slots[i].load_ts;
    }

    if (currIn < lastIn) {
      lastIn = currIn;
       evictIndex = i;
    }

  }
  return evictIndex;
}

// parse input for write allocate, write through, and eviction
// return 0 if all flags are parsed without error; else, return 1 (error code)
bool handleFlags(std::string wa, std::string wt, std::string ev, Flag &flags) {
  
  // handle write allocate flag
  if (wa == "write-allocate") {
    flags.writeAlloc = true;
  }
  else if(wa == "no-write-allocate") {
    flags.writeAlloc = false;
  }
  else {
    std::cerr << "invalid write-alloc arg" << std::endl;
    return 1;
  }

  // handle write through flag
  if (wt == "write-through") {
    flags.writeBack = false;
  }
  else if(wt == "write-back") {
    flags.writeBack = true;
  }
  else {
    std::cerr << "invalid write-through arg" << std::endl;
    return 1; 
  }

  // handle eviction flag
  if (ev == "lru") {
    flags.lru = true;
  }
  else if(ev == "fifo") {
    flags.lru = false;
  }
  else {
    std::cerr << "invalid eviction arg" << std::endl;
    return 1; 
  }

  //if writeallocate is false and writeback is true
  if (!flags.writeAlloc && flags.writeBack) {
    std::cerr << "write should allocate and also write back" << std::endl;
    return 1;
  }

  // all args handled, successfully set flags
  return 0;
}

// handle case when store command results in cache hit
void handleStoreHit(CacheData &cacheData, bool writeBack, Set &set, int hit) {
  // update storeHits and totalCycles
  cacheData.storeHits++;
  cacheData.totalCycles++;
  // handle writeBack
  if (writeBack) {
    set.slots[hit].dirty = true;
  } 
  // handle writethrough
  else {
    cacheData.totalCycles += 100;
  }
}

// handle case when cache call results in hit
void handleCacheHit(bool loaded, CacheData &cacheData, bool writeBack, Set &set, int hit) {
  // load only needs to increase count because it is already on the cache
  if(loaded) {
    cacheData.loadHits++;
    cacheData.totalCycles++;
  }
  // handle store case
  else {
    handleStoreHit(cacheData, writeBack, set, hit);
  }
  set.slots[hit].access_ts = cacheData.timestamps;
}

// handle case when cache load call results in miss
void handleCacheMissLoad(CacheData &cacheData, uint32_t blkSize, Set &set, bool lru, uint32_t tag) {
  cacheData.loadMisses++;
  cacheData.totalCycles++;
  cacheData.totalCycles += blkSize/4 * 100;
  //try to insert in new slot thats empty
  int hit = findEmpty(set);
  //if no empty spots then we have to free up one with FIFO logic
  if (hit < 0) {
    hit = findEvict(set, lru);
    if (set.slots[hit].dirty) {
      cacheData.totalCycles += blkSize/4 * 100;

    }
  }

  // update cache to hold address
  set.slots[hit].valid = true;
  set.slots[hit].tag = tag;
  set.slots[hit].access_ts = cacheData.timestamps;
  set.slots[hit].dirty = false;
  set.slots[hit].load_ts = cacheData.timestamps;
}

// update cache and cacheData for when store results in a cache miss and write alloc is true
void handleCacheMissStoreWriteAlloc(CacheData &cacheData, uint32_t blkSize, Set &set, const Flag &flags, uint32_t tag) {
  
  // check if there is empty slot in cache to insert to
  cacheData.totalCycles += blkSize/4 * 100 + 1;
  int hit = findEmpty(set);

  // no empty slot; handle eviction
  if (hit < 0) {
    hit = findEvict(set, flags.lru);
    if (set.slots[hit].dirty) {
      cacheData.totalCycles += blkSize/4 * 100;
    }
  }

  // update cache to hold address
  set.slots[hit].valid = true;
  set.slots[hit].tag = tag;
  set.slots[hit].access_ts = cacheData.timestamps;
  set.slots[hit].dirty = false;
  set.slots[hit].load_ts = cacheData.timestamps;

  // writeBack
  if (flags.writeBack) {
    set.slots[hit].dirty = true;
  } 
  // writethrough
  else {
    set.slots[hit].dirty = false;
    cacheData.totalCycles += 100;
  }
}

// given a command line, parses command and updates CacheData accordingly
void updateCache(Cache &newCache, std::string line, CacheData &cacheData, const Flag &flags, uint32_t offsetBits, uint32_t indexBits, uint32_t blkSize) {

  // parse command
  char operation;
  uint32_t address;
  int data; // ignored for this assignment

  //string stream to help parse the input
  std::stringstream ss(line);
  ss >> operation;
  ss >> std::hex >> address;
  ss >> std::dec >> data;

  // Extract index and tag 
  uint32_t index = (address >> offsetBits) & ((1u << indexBits) - 1);
  uint32_t tag = address >> (offsetBits + indexBits);

  Set &set = newCache.sets[index];
  cacheData.timestamps++;

  //update load or store values
  bool loaded = (operation == 'l');
  if(loaded) {
    cacheData.totalLoads++;
  } 
  else {
    cacheData.totalStores++;
  }

  // check if address is a hit
  int hit = -1;
  for(int i = 0; i <  (int)set.slots.size(); i++) {
    if(set.slots[i].valid && set.slots[i].tag == tag) {
      hit = i;
      break;
    }
  }

  // Cache hit
  if(hit >= 0) {
    handleCacheHit(loaded, cacheData, flags.writeBack, set, hit);
  }
  // Cache miss
  else {
    if(loaded) {
      handleCacheMissLoad(cacheData, blkSize, set, flags.lru, tag);
    } 
    else {
      // increase counter
      cacheData.storeMisses++;
      
      // handle store miss with writeAlloc
      if(flags.writeAlloc) {
        handleCacheMissStoreWriteAlloc(cacheData, blkSize, set, flags, tag);
      }
      else {
        //no write alloc so skip
        cacheData.totalCycles += 100 + 1;
      }
    }
  }  
}

// print cacheData in accordance to assignment specifications
void printOutputs(CacheData cacheData) {
  std::cout << "Total loads: " << cacheData.totalLoads << std::endl;
  std::cout << "Total stores: " << cacheData.totalStores << std::endl;
  std::cout << "Load hits: " << cacheData.loadHits << std::endl;
  std::cout << "Load misses: " << cacheData.loadMisses << std::endl;
  std::cout << "Store hits: " << cacheData.storeHits << std::endl;
  std::cout << "Store misses: " << cacheData.storeMisses << std::endl;
  std::cout << "Total cycles: " << cacheData.totalCycles << std::endl;
}

int main( int argc, char **argv ) {
  //ERROR HANDLING: incorrect number of args entered
  if (argc != 7) {
    std::cerr << "Expected 6 elements but got " << (argc - 1) << std::endl;
    return 1;
  }

  //arguments from binary to decimal
  uint32_t numSets = atoi(argv[1]);
  uint32_t numBlocks = atoi(argv[2]);
  uint32_t blkSize = atoi(argv[3]);

  //ERROR HANDLING: numSets, numBlocks, or blkSize entered incorrectly
  if (!isPowerOf2(numSets) || !isPowerOf2(numBlocks) || !isPowerOf2(blkSize)) {
    std::cerr << "All arguments must be powers of 2" << std::endl;
    return 1;
  }

  //ERROR HANDLING: blkSize entered incorrectly
  if (blkSize < 4) {
    std::cerr << "Block size must be greater than 4"<< std::endl;
    return 1;
  }

  //intialize flags corresponding to user input
  Flag flags;
  // ERROR HANDLING: incorrect args inputted
  if (handleFlags(argv[4], argv[5], argv[6], flags) == 1) {
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

  CacheData cacheData;

  //parse the traces
  std::string line;
  while(std::getline(std::cin, line)) {
    if(!line.empty()) {
      updateCache(newCache, line, cacheData, flags, offsetBits, indexBits, blkSize);      
    }  
  }

  // print out cache data
  printOutputs(cacheData);

  // successful run, return 0
  return 0;
}
