#!/bin/bash

TRACE=$1

SETS=(128 256 512)
BLOCKS=(1 4 8)
BLKSIZE=16
WRITE_POLICIES=("write-allocate write-back" "write-allocate write-through" "no-write-allocate write-through")
EVICTIONS=("lru" "fifo")
FA_BLOCKS=(128 256 512)

ALL_DATA=""

collect_result() {
  local sets=$1
  local blocks=$2
  local wa=$3
  local wt=$4
  local evict=$5

  local output
  output=$(./csim $sets $blocks $BLKSIZE $wa $wt $evict < $TRACE)

  local totalLoads=$(echo "$output" | grep "Total loads" | awk '{print $3}')
  local totalStores=$(echo "$output" | grep "Total stores" | awk '{print $3}')
  local loadHits=$(echo "$output" | grep "Load hits" | awk '{print $3}')
  local storeHits=$(echo "$output" | grep "Store hits" | awk '{print $3}')
  local totalCycles=$(echo "$output" | grep "Total cycles" | awk '{print $3}')

  local totalOps=$((totalLoads + totalStores))
  local totalHits=$((loadHits + storeHits))
  local hitRate=$(echo "scale=6; $totalHits / $totalOps" | bc)

  local label="sets=$sets|blocks=$blocks|wa=$wa|wt=$wt|evict=$evict"
  ALL_DATA+="$totalCycles $hitRate $sets $label\n"
}

for sets in "${SETS[@]}"; do
  for blocks in "${BLOCKS[@]}"; do
    for policy in "${WRITE_POLICIES[@]}"; do
      for evict in "${EVICTIONS[@]}"; do
        wa=$(echo $policy | cut -d' ' -f1-2)
        wt=$(echo $policy | cut -d' ' -f3)
        collect_result $sets $blocks "$wa" "$wt" $evict
      done
    done
  done
done

for blocks in "${FA_BLOCKS[@]}"; do
  for policy in "${WRITE_POLICIES[@]}"; do
    for evict in "${EVICTIONS[@]}"; do
      wa=$(echo $policy | cut -d' ' -f1-2)
      wt=$(echo $policy | cut -d' ' -f3)
      collect_result 1 $blocks "$wa" "$wt" $evict
    done
  done
done

echo "=== Results per Cache Size (ranked by total cycles) ==="
for size in 1 128 256 512; do
  echo ""
  echo "  -- Cache sets=$size --"
  printf "    %-15s %-10s %s\n" "Total Cycles" "Hit Rate" "Configuration"
  echo -e "$ALL_DATA" | grep " $size " | sort -n | while read cycles hitrate sz lbl; do
    printf "    %-15s %-10s %s\n" "$cycles" "$hitrate" "$(echo $lbl | tr '|' ' ')"
  done
done