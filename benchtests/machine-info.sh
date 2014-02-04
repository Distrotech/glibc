#!/bin/sh
# Simple script to get some basic machine information

echo "{"
sed -n  -e 's/vendor_id[ \t]\+: \(.*\)/  "vendor-id": "\1",/p' \
	-e 's/cpu family[ \t]\+: \(.*\)/  "cpu-family": \1,/p' \
	-e 's/\(model\)[ \t]\+: \(.*\)/  "\1": \2,/p' \
	-e 's/cpu cores[ \t]\+: \(.*\)/  "cpu-cores": \1,/p' \
	-e 's/cache size[ \t]\+: \([0-9]\+\).*/  "cache-size": \1,/p' \
	-e 's/MemTotal:[ \t]\+\([0-9]\+\).*/  "memory": \1,/p' \
	-e 's/SwapTotal:[ \t]\+\([0-9]\+\).*/  "swap": \1,/p' \
	 /proc/cpuinfo /proc/meminfo | sort -u
echo "  \"processors\": $(grep -c 'processor' /proc/cpuinfo)"
echo "}"
