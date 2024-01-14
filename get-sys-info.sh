#! /bin/bash

txtcyn='\e[0;36m' # Cyan Text
txtwht='\e[0;37m' # White Text

# create directory for system info
mkdir -p sysinfo

echo -e ${txtcyn}Reading system info ...${txtwht}

# gather system info
cat /proc/meminfo > sysinfo/meminfo.txt
cat /proc/cpuinfo > sysinfo/cpuinfo.txt
lscpu > sysinfo/lscpu.txt
lshw > sysinfo/lshw.txt

echo -e ${txtcyn}Saved system info in sysinfo${txtwht}

