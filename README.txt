Annabelle Li-Kroeger - completed FIFO logic and refactored cache; helped with report
James To - completed original cache and Make file for lru; helped with report

Best cache report:
To compare cache sizes, a couple factors were considered.

1. Associativity. We test direct-mapped, 4-way associative, 8-way associative, and fully associative caches.
2. Cache size. We varied the number of sets contained in the cache to 128, 256, and 512.
3. Write policy. The three valid combinations (write-allocate + write-back, write-allocate + write-through, and 
no-write-allocate + write-through) were tested.
4. Eviction policy. We tried both LRU and FIFO and checked the results.

To test each factor, we put all possible combinations into a bash file and ran all the commands and examined the outputs. The
test files whose outputs we looked at were gcc.trace and swim.trace as these are real files. For each configuration, we
compared the total cycle count and the hit rate. Lowest total cycles is the most important metric as it tells us how many 
clock cycles it took for the commands to run. However, we also examine the hit rates because we want to see which 
combination results in more accurate stores and loads from the cache. The hit rate is typically reflected in the total 
cycles as cache misses cost many more cycles than hits. The results for the gcc.trace files are shown below:

=== Results per Cache Size (ranked by total cycles) ===

  -- Cache sets=1 --
    Total Cycles    Hit Rate   Configuration
    9880883         .974185    sets=1 blocks=512 wa=write-allocate write-back wt= evict=lru
    10740883        .971478    sets=1 blocks=512 wa=write-allocate write-back wt= evict=fifo
    10802483        .971092    sets=1 blocks=256 wa=write-allocate write-back wt= evict=lru
    12461283        .965713    sets=1 blocks=256 wa=write-allocate write-back wt= evict=fifo
    12654883        .964449    sets=1 blocks=128 wa=write-allocate write-back wt= evict=lru
    15673683        .954964    sets=1 blocks=128 wa=write-allocate write-back wt= evict=fifo
    23010843        .922454    sets=1 blocks=512 wa=no-write-allocate write-through wt= evict=lru
    23399138        .916479    sets=1 blocks=512 wa=no-write-allocate write-through wt= evict=fifo
    23598409        .917984    sets=1 blocks=256 wa=no-write-allocate write-through wt= evict=lru
    24318667        .909551    sets=1 blocks=256 wa=no-write-allocate write-through wt= evict=fifo
    24368165        .909890    sets=1 blocks=128 wa=no-write-allocate write-through wt= evict=lru
    25589083        .974185    sets=1 blocks=512 wa=write-allocate write-through wt= evict=lru
    25937264        .897421    sets=1 blocks=128 wa=no-write-allocate write-through wt= evict=fifo
    26147483        .971478    sets=1 blocks=512 wa=write-allocate write-through wt= evict=fifo
    26227083        .971092    sets=1 blocks=256 wa=write-allocate write-through wt= evict=lru
    27336683        .965713    sets=1 blocks=256 wa=write-allocate write-through wt= evict=fifo
    27597483        .964449    sets=1 blocks=128 wa=write-allocate write-through wt= evict=lru
    29553883        .954964    sets=1 blocks=128 wa=write-allocate write-through wt= evict=fifo

  -- Cache sets=128 --
    Total Cycles    Hit Rate   Configuration
    9277683         .975766    sets=128 blocks=8 wa=write-allocate write-back wt= evict=lru
    9781683         .974123    sets=128 blocks=8 wa=write-allocate write-back wt= evict=fifo
    10071683        .973508    sets=128 blocks=4 wa=write-allocate write-back wt= evict=lru
    10901683        .970854    sets=128 blocks=4 wa=write-allocate write-back wt= evict=fifo
    22833330        .924261    sets=128 blocks=8 wa=no-write-allocate write-through wt= evict=lru
    23060117        .921581    sets=128 blocks=8 wa=no-write-allocate write-through wt= evict=fifo
    23124725        .921672    sets=128 blocks=4 wa=no-write-allocate write-through wt= evict=lru
    23504762        .917569    sets=128 blocks=4 wa=no-write-allocate write-through wt= evict=fifo
    25263083        .975766    sets=128 blocks=8 wa=write-allocate write-through wt= evict=lru
    25601883        .974123    sets=128 blocks=8 wa=write-allocate write-through wt= evict=fifo
    25728683        .973508    sets=128 blocks=4 wa=write-allocate write-through wt= evict=lru
    26276283        .970854    sets=128 blocks=4 wa=write-allocate write-through wt= evict=fifo
    33377283        .890857    sets=128 blocks=1 wa=write-allocate write-back wt= evict=fifo
    33377283        .890857    sets=128 blocks=1 wa=write-allocate write-back wt= evict=lru
    35964382        .827655    sets=128 blocks=1 wa=no-write-allocate write-through wt= evict=fifo
    35964382        .827655    sets=128 blocks=1 wa=no-write-allocate write-through wt= evict=lru
    42777483        .890857    sets=128 blocks=1 wa=write-allocate write-through wt= evict=fifo
    42777483        .890857    sets=128 blocks=1 wa=write-allocate write-through wt= evict=lru

  -- Cache sets=256 --
    Total Cycles    Hit Rate   Configuration
    8562483         .976906    sets=256 blocks=8 wa=write-allocate write-back wt= evict=lru
    8830083         .976074    sets=256 blocks=8 wa=write-allocate write-back wt= evict=fifo
    9344483         .975498    sets=256 blocks=4 wa=write-allocate write-back wt= evict=lru
    9845283         .973888    sets=256 blocks=4 wa=write-allocate write-back wt= evict=fifo
    20312483        .938687    sets=256 blocks=1 wa=write-allocate write-back wt= evict=fifo
    20312483        .938687    sets=256 blocks=1 wa=write-allocate write-back wt= evict=lru
    22735952        .925165    sets=256 blocks=8 wa=no-write-allocate write-through wt= evict=lru
    22850821        .923964    sets=256 blocks=8 wa=no-write-allocate write-through wt= evict=fifo
    22865216        .923885    sets=256 blocks=4 wa=no-write-allocate write-through wt= evict=lru
    23102502        .920569    sets=256 blocks=4 wa=no-write-allocate write-through wt= evict=fifo
    25027883        .976906    sets=256 blocks=8 wa=write-allocate write-through wt= evict=lru
    25199483        .976074    sets=256 blocks=8 wa=write-allocate write-through wt= evict=fifo
    25318283        .975498    sets=256 blocks=4 wa=write-allocate write-through wt= evict=lru
    25650283        .973888    sets=256 blocks=4 wa=write-allocate write-through wt= evict=fifo
    29004091        .882606    sets=256 blocks=1 wa=no-write-allocate write-through wt= evict=fifo
    29004091        .882606    sets=256 blocks=1 wa=no-write-allocate write-through wt= evict=lru
    32911483        .938687    sets=256 blocks=1 wa=write-allocate write-through wt= evict=fifo
    32911483        .938687    sets=256 blocks=1 wa=write-allocate write-through wt= evict=lru

  -- Cache sets=512 --
    Total Cycles    Hit Rate   Configuration
    7609683         .977538    sets=512 blocks=8 wa=write-allocate write-back wt= evict=lru
    7766483         .977075    sets=512 blocks=8 wa=write-allocate write-back wt= evict=fifo
    8607683         .976722    sets=512 blocks=4 wa=write-allocate write-back wt= evict=lru
    8875283         .975874    sets=512 blocks=4 wa=write-allocate write-back wt= evict=fifo
    14469283        .958387    sets=512 blocks=1 wa=write-allocate write-back wt= evict=fifo
    14469283        .958387    sets=512 blocks=1 wa=write-allocate write-back wt= evict=lru
    22657603        .925644    sets=512 blocks=8 wa=no-write-allocate write-through wt= evict=lru
    22716294        .925147    sets=512 blocks=8 wa=no-write-allocate write-through wt= evict=fifo
    22754658        .924891    sets=512 blocks=4 wa=no-write-allocate write-through wt= evict=lru
    22878289        .923575    sets=512 blocks=4 wa=no-write-allocate write-through wt= evict=fifo
    24897483        .977538    sets=512 blocks=8 wa=write-allocate write-through wt= evict=lru
    24993083        .977075    sets=512 blocks=8 wa=write-allocate write-through wt= evict=fifo
    25065883        .976722    sets=512 blocks=4 wa=write-allocate write-through wt= evict=lru
    25240683        .975874    sets=512 blocks=4 wa=write-allocate write-through wt= evict=fifo
    25437594        .903595    sets=512 blocks=1 wa=no-write-allocate write-through wt= evict=fifo
    25437594        .903595    sets=512 blocks=1 wa=no-write-allocate write-through wt= evict=lru
    28847883        .958387    sets=512 blocks=1 wa=write-allocate write-through wt= evict=fifo
    28847883        .958387    sets=512 blocks=1 wa=write-allocate write-through wt= evict=lru

The results for swim.trace are shown below:

=== Results per Cache Size (ranked by total cycles) ===

  -- Cache sets=1 --
    Total Cycles    Hit Rate   Configuration
    9548793         .958956    sets=1 blocks=512 wa=write-allocate write-back wt= evict=lru
    9650267         .909549    sets=1 blocks=512 wa=no-write-allocate write-through wt= evict=lru
    9759766         .908312    sets=1 blocks=512 wa=no-write-allocate write-through wt= evict=fifo
    10001569        .905667    sets=1 blocks=256 wa=no-write-allocate write-through wt= evict=lru
    11006605        .896837    sets=1 blocks=256 wa=no-write-allocate write-through wt= evict=fifo
    11061593        .948824    sets=1 blocks=512 wa=write-allocate write-back wt= evict=fifo
    11301193        .946456    sets=1 blocks=256 wa=write-allocate write-back wt= evict=lru
    11974247        .887675    sets=1 blocks=128 wa=no-write-allocate write-through wt= evict=lru
    12699572        .879462    sets=1 blocks=128 wa=no-write-allocate write-through wt= evict=fifo
    13042793        .936730    sets=1 blocks=256 wa=write-allocate write-back wt= evict=fifo
    13533293        .958956    sets=1 blocks=512 wa=write-allocate write-through wt= evict=lru
    13622393        .931637    sets=1 blocks=128 wa=write-allocate write-back wt= evict=lru
    14762093        .948824    sets=1 blocks=512 wa=write-allocate write-through wt= evict=fifo
    15049293        .946456    sets=1 blocks=256 wa=write-allocate write-through wt= evict=lru
    16228893        .936730    sets=1 blocks=256 wa=write-allocate write-through wt= evict=fifo
    16444393        .916854    sets=1 blocks=128 wa=write-allocate write-back wt= evict=fifo
    16846493        .931637    sets=1 blocks=128 wa=write-allocate write-through wt= evict=lru
    18639293        .916854    sets=1 blocks=128 wa=write-allocate write-through wt= evict=fifo

  -- Cache sets=128 --
    Total Cycles    Hit Rate   Configuration
    8991593         .961414    sets=128 blocks=8 wa=write-allocate write-back wt= evict=lru
    9549993         .959039    sets=128 blocks=4 wa=write-allocate write-back wt= evict=lru
    9557203         .910766    sets=128 blocks=8 wa=no-write-allocate write-through wt= evict=lru
    9603171         .910281    sets=128 blocks=8 wa=no-write-allocate write-through wt= evict=fifo
    9661042         .909377    sets=128 blocks=4 wa=no-write-allocate write-through wt= evict=lru
    9679193         .956875    sets=128 blocks=8 wa=write-allocate write-back wt= evict=fifo
    9756579         .908381    sets=128 blocks=4 wa=no-write-allocate write-through wt= evict=fifo
    10935593        .949794    sets=128 blocks=4 wa=write-allocate write-back wt= evict=fifo
    13235293        .961414    sets=128 blocks=8 wa=write-allocate write-through wt= evict=lru
    13523293        .959039    sets=128 blocks=4 wa=write-allocate write-through wt= evict=lru
    13785693        .956875    sets=128 blocks=8 wa=write-allocate write-through wt= evict=fifo
    14644493        .949794    sets=128 blocks=4 wa=write-allocate write-through wt= evict=fifo
    18024090        .831981    sets=128 blocks=1 wa=no-write-allocate write-through wt= evict=fifo
    18024090        .831981    sets=128 blocks=1 wa=no-write-allocate write-through wt= evict=lru
    23141593        .866517    sets=128 blocks=1 wa=write-allocate write-back wt= evict=fifo
    23141593        .866517    sets=128 blocks=1 wa=write-allocate write-back wt= evict=lru
    24744093        .866517    sets=128 blocks=1 wa=write-allocate write-through wt= evict=fifo
    24744093        .866517    sets=128 blocks=1 wa=write-allocate write-through wt= evict=lru

  -- Cache sets=256 --
    Total Cycles    Hit Rate   Configuration
    8509993         .961819    sets=256 blocks=8 wa=write-allocate write-back wt= evict=lru
    8782393         .960104    sets=256 blocks=8 wa=write-allocate write-back wt= evict=fifo
    9009593         .961311    sets=256 blocks=4 wa=write-allocate write-back wt= evict=lru
    9549214         .910868    sets=256 blocks=8 wa=no-write-allocate write-through wt= evict=lru
    9564400         .910697    sets=256 blocks=8 wa=no-write-allocate write-through wt= evict=fifo
    9569598         .910647    sets=256 blocks=4 wa=no-write-allocate write-through wt= evict=lru
    9611162         .910185    sets=256 blocks=4 wa=no-write-allocate write-through wt= evict=fifo
    9655593         .956961    sets=256 blocks=4 wa=write-allocate write-back wt= evict=fifo
    11231805        .892335    sets=256 blocks=1 wa=no-write-allocate write-through wt= evict=fifo
    11231805        .892335    sets=256 blocks=1 wa=no-write-allocate write-through wt= evict=lru
    13186093        .961819    sets=256 blocks=8 wa=write-allocate write-through wt= evict=lru
    13247693        .961311    sets=256 blocks=4 wa=write-allocate write-through wt= evict=lru
    13394093        .960104    sets=256 blocks=8 wa=write-allocate write-through wt= evict=fifo
    13775293        .956961    sets=256 blocks=4 wa=write-allocate write-through wt= evict=fifo
    14381193        .929259    sets=256 blocks=1 wa=write-allocate write-back wt= evict=fifo
    14381193        .929259    sets=256 blocks=1 wa=write-allocate write-back wt= evict=lru
    17134893        .929259    sets=256 blocks=1 wa=write-allocate write-through wt= evict=fifo
    17134893        .929259    sets=256 blocks=1 wa=write-allocate write-through wt= evict=lru

  -- Cache sets=512 --
    Total Cycles    Hit Rate   Configuration
    7753193         .961987    sets=512 blocks=8 wa=write-allocate write-back wt= evict=lru
    7848793         .961625    sets=512 blocks=8 wa=write-allocate write-back wt= evict=fifo
    8521593         .961753    sets=512 blocks=4 wa=write-allocate write-back wt= evict=lru
    8796793         .960035    sets=512 blocks=4 wa=write-allocate write-back wt= evict=fifo
    9543218         .910931    sets=512 blocks=8 wa=no-write-allocate write-through wt= evict=lru
    9550414         .910858    sets=512 blocks=4 wa=no-write-allocate write-through wt= evict=lru
    9552003         .910809    sets=512 blocks=8 wa=no-write-allocate write-through wt= evict=fifo
    9565596         .910674    sets=512 blocks=4 wa=no-write-allocate write-through wt= evict=fifo
    10067758        .904423    sets=512 blocks=1 wa=no-write-allocate write-through wt= evict=fifo
    10067758        .904423    sets=512 blocks=1 wa=no-write-allocate write-through wt= evict=lru
    11479593        .947178    sets=512 blocks=1 wa=write-allocate write-back wt= evict=fifo
    11479593        .947178    sets=512 blocks=1 wa=write-allocate write-back wt= evict=lru
    13165693        .961987    sets=512 blocks=8 wa=write-allocate write-through wt= evict=lru
    13194093        .961753    sets=512 blocks=4 wa=write-allocate write-through wt= evict=lru
    13209693        .961625    sets=512 blocks=8 wa=write-allocate write-through wt= evict=fifo
    13402493        .960035    sets=512 blocks=4 wa=write-allocate write-through wt= evict=fifo
    14961693        .947178    sets=512 blocks=1 wa=write-allocate write-through wt= evict=fifo
    14961693        .947178    sets=512 blocks=1 wa=write-allocate write-through wt= evict=lru

Based on these results, the clear "most efficient" cache configuration is 512 sets with 8 blocks, write-allocate, write-back,
and lru eviction.
In terms of associativity, higher associativity cause better results but at the cost of more overhead (discussed later).
However, fully associated caches actually performed worse than partially associated caches because the fully associative caches 
tested here hold less total data than the set-associative configurations. For example, 1 × 512 × 16 = 8 KB vs 128 × 8 × 16 = 16
KB. This smaller capacity results in more misses, thus increasing total cycle cost.
- write-back beats write-through because write-through would require a write to memory at every hit and miss, whereas write-
back only requires a write to memory when a dirty block is evicted. This decreases total cycle cost because you are accessing
memory less frequently. It is important to note that write-back and write-through have identical hit rates because write-back
only increases efficiency in the sense that you don't need to access memory for a hit; it doesn't change the total amount of hits
that the program has.
- write-allocation beats no-write-allocation because it increases hit rate, thus making misses less frequent. It increases hit
rate by immediately writing stored data to the cache, which is important because codes often have temporal locality 
(meaning data that is stored will likely be accessed in the near future).
- lru typically beats fifo because data that is used frequently (temporal locality) may be loaded earlier than data that is
loaded but then only used once. By refreshing the order every time data is accessed, the user may utilize temporal locality
better.
Thus, we know that the best setting should have write-back, write-allocation, and lru (which is reflected in the data).
However, we need to see if the increased efficiency justifies the increased cache size. In this case,
going from 256 to 512 sets provides about an 11% increase in efficiency for double the space. We also need to consider
whether the increased efficiency justifies the increased overhead in the case of having 8 blocks instead of 4. The best-case
8 block for cache size 512 provides about 9% efficiency increase for about double the overhead. For high performance (which in
our opinion is more important than cost), 512 sets with 8 blocks is the best option. However, for a more cost-effective option,
256 with 4 blocks is sufficient as it only has 13% performance decrease for half the hardware.
We prefer more efficiency over cost, effectiveness. Thus, our winning configuration (512 sets, 8 blocks, 16 byte blocks) 
has a data capacity of 65,536 bytes (64 KB). Including overhead — each block requires 19 tag bits, 1 valid bit, and 1 dirty 
bit — total overhead is 10,752 bytes (~10.5 KB), bringing the total hardware cost to ~74.5 KB. The runner-up (256 sets, 
4 blocks) costs ~37.25 KB total.