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

// parse input for write allocate, write through, and eviction
// place corresponding flag into flag_init
// return 0 if all flags are parsed without error; else, return 1 (error code)
bool handleFlags(std::string wa, std::string wt, std::string ev, bool flag_init[]) {
  
  // handle write allocate flag
  if (wa == "write-allocate") {
    flag_init[0] = true;
  }
  else if(wa == "no-write-allocate") {
    flag_init[0] = false;
  }
  else {
    std::cerr << "invalid write-alloc arg" << std::endl;
    return 1;
  }

  // handle write through flag
  if (wt == "write-through") {
    flag_init[1] = false;
  }
  else if(wt == "write-back") {
    flag_init[1] = true;
  }
  else {
    std::cerr << "invalid write-through arg" << std::endl;
    return 1; 
  }

  // handle eviction flag
  if (ev == "lru") {
    flag_init[2] = true;
  }
  else if(ev == "fifo") {
    flag_init[2] = false;
  }
  else {
    std::cerr << "invalid eviction arg" << std::endl;
    return 1; 
  }

  //if writeallocate is false and writeback is true
  if (!flag_init[0] && flag_init[1]) {
    std::cerr << "write should allocate and also write back" << std::endl;
    return 1;
  }

  // all args handled, successfully set flags
  return 0;
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
    std::cerr << "All arguments must be powers of 2." << std::endl;
    return 1;
  }

  //ERROR HANDLING: blkSize entered incorrectly
  if (blkSize < 4) {
    std::cerr << "Block size must be greater than 4 "<< std::endl;
    return 1;
  }

  //intialize writeAlloc writeBack and lru
  bool flag_init[3];
  // ERROR HANDLING: incorrect args inputted
  if (handleFlags(argv[4], argv[5], argv[6], flag_init) == 1) {
    return 1;
  }
  bool writeAlloc = flag_init[0];
  bool writeBack = flag_init[1];
  bool lru = flag_init[2];

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
      timestamps++;
  
      //update load or store values
      bool loaded = (operation == 'l');

      if(loaded) {
        totalLoads++;
      } 
      else {
        totalStores++;
      }

      int hit = -1;
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







