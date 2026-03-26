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


//find oldest slot using FIFO methodology
int findEvict(const Set &set, bool lru) {
  uint32_t evictIndex = 0;
  uint32_t lastIn = set.slots[0].access_ts;
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
  uint64_t totalLoads = 0;
  uint64_t totalStores = 0;
  uint64_t loadHits = 0;
  uint64_t loadMisses = 0;
  uint64_t storeHits = 0;
  uint64_t storeMisses = 0;
  uint64_t totalCycles = 0;
  uint64_t timestamps = 0;
  

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
      bool loaded = (operation == 'l');

      if(loaded) {
        totalLoads++;
      } 
      else {
        totalStores++;
      }

      int hit = -67;
      for(int i = 0; i <  (int)set.slots.size(); i++) {
        if(set.slots[i].valid && set.slots[i].tag == tag) {
          hit = i;
          break;
        }

      }

      if(hit >= 0) {
      // Cache hit
        if(loaded) {
          loadHits++;
          totalCycles++;
          
        } 
        else {
          storeHits++;
          totalCycles++;
          if (writeBack) {
            set.slots[hit].dirty = true;
          } else {
            // writethrough
            totalCycles += 100;
          }
        }

        
        set.slots[hit].access_ts = timestamps;

      } 
      else {
        // Cache miss
        if(loaded) {
          loadMisses++;
          totalCycles++;
          totalCycles += blkSize/4 * 100;
          //try to insert in new slot thats empty
          hit = findEmpty(set);
          //if no empty spots then we have to free up one with FIFO logic
          if (hit < 0) {
            hit = findEvict(set, lru);

            if (set.slots[hit].dirty) {
              totalCycles += blkSize/4 * 100;

            }
          }


          set.slots[hit].valid = true;
          set.slots[hit].tag = tag;
          set.slots[hit].access_ts = timestamps;
          set.slots[hit].dirty = false;
          set.slots[hit].load_ts = timestamps;


        } 
        else {
          storeMisses++;
          
          if(writeAlloc) {
            totalCycles += blkSize/4 * 100 + 1;
            hit = findEmpty(set);

            if (hit < 0) {
              hit = findEvict(set, lru);

              if (set.slots[hit].dirty) {
                totalCycles += blkSize/4 * 100;

              }
            }

            set.slots[hit].valid = true;
            set.slots[hit].tag = tag;
            set.slots[hit].access_ts = timestamps;
            set.slots[hit].dirty = false;
            set.slots[hit].load_ts = timestamps;

            if (writeBack) {
              set.slots[hit].dirty = true;
            } else {
              // writethrough
              set.slots[hit].dirty = false;
              totalCycles += 100;
            }


        }
        else {

          //no write alloc so skip
          totalCycles += 100 + 1;
        }


      }


    }
    
  }

  

  

  
  }

  std::cout << "Total loads: " << totalLoads << std::endl;
  std::cout << "Total stores: " << totalStores << std::endl;
  std::cout << "Load hits: " << loadHits << std::endl;
  std::cout << "Load misses: " << loadMisses << std::endl;
  std::cout << "Store hits: " << storeHits << std::endl;
  std::cout << "Store misses: " << storeMisses << std::endl;
  std::cout << "Total cycles: " << totalCycles << std::endl;

  return 0;
}







