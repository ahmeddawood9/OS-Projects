#!/bin/bash

read cpu user nice system idle iowait irq softirq steal guest guest_nice < /proc/stat

prev_idle=$idle
prev_total=$((user + nice + system + idle + iowait + irq + softirq + steal))

sleep 1

read cpu user nice system idle iowait irq softirq steal guest guest_nice < /proc/stat

idle=$((idle))
total=$((user + nice + system + idle + iowait + irq + softirq + steal))

diff_idle=$((idle - prev_idle))
diff_total=$((total - prev_total))

cpu_usage=$(( (100 * (diff_total - diff_idle)) / diff_total ))


#this is for memory 
mem_total=$(grep MemTotal /proc/meminfo | awk '{print $2}')
mem_free=$(grep MemFree /proc/meminfo | awk '{print $2}')
buffers=$(grep Buffers /proc/meminfo | awk '{print $2}')
cached=$(grep "^Cached:" /proc/meminfo | awk '{print $2}')

mem_used=$((mem_total - mem_free - buffers - cached))
mem_usage=$((100 * mem_used / mem_total))

#System Load Average 
load=$(awk '{print $1}' /proc/loadavg)

#for disk usage 
disk_usage=$(df / | awk 'NR==2 {print $5}' | tr -d '%')

#generating json output
timestamp=$(date +"%Y-%m-%d %H:%M:%S")

cat <<EOF > data.json
{
  "timestamp": "$timestamp",
  "cpu": $cpu_usage,
  "memory": $mem_usage,
  "load": $load,
  "disk": $disk_usage
}
EOF

echo "$timestamp,$cpu_usage,$mem_usage,$load,$disk_usage" >> history.log
