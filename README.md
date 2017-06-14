Basic implementation for memory heirarchy with multilevel cache. Private L1, L2 and shared L3 as last level cache.
Supports two configurations to have the L3 as inclusive or victim (exclusive) cache.
The simulation uses address trace generated from Gem5

Usage: memmodel [-i/-e] [-n <number>] tracefilename
Options
i for inclusive LLC
e for exclusive cache
n for number of cores
No coherency implemented so only multiprogramming workload wihtout coherency requirement can be simulated
