#!/bin/sh
# Simple script to get some basic machine information

{ sed -n -e 's/\(vendor_id\)[ \t]\+:/1. \1:/p' \
	 -e 's/\(cpu family\)[ \t]\+:/2. \1:/p' \
	 -e 's/\(model\)[ \t]\+:/3. \1:/p' \
	 -e 's/\(cpu cores\)[ \t]\+:/4. \1:/p' \
	 -e 's/\(cache size\)[ \t]\+:/6. \1:/p' \
	 -e 's/MemTotal:[ \t]\+\(.*\)/7. memory: \1/p' \
	 -e 's/SwapTotal:[ \t]\+\(.*\)/8. swap: \1/p' \
	 /proc/cpuinfo /proc/meminfo; \
	echo 5. processors: $(grep -c 'processor' /proc/cpuinfo); } | 
	sort -u
