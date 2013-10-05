#!/bin/sh

workspace="/home/ndiegues/stamp-rtm/"

cd tl2;
make clean; make;
cd ../TinySTM;
make clean; make;
cd ../swisstm;
make clean; make;
cd ..

config[1]="rtm"
config[2]="seq"
config[3]="tl2"
config[4]="tinystm"
config[5]="swisstm"
config[6]="rtmmutex"
config[7]="rtmspin"
config[8]="seq"
config[9]="seq"
config[10]="seq"
config[11]="seq"
config[12]="seq"
config[13]="seq"
config[14]="seq"
config[15]="seq"
config[16]="seq"
alias[17]="rtmssync"
alias[18]="rtmssync"
alias[19]="rtmssync"
alias[20]="rtmssync"
alias[21]="rtmssync"
alias[22]="rtmssync"
alias[23]="rtmssync"
alias[24]="rtmssync"
alias[25]="rtmssync"


alias[8]="MCS_LOCKS"
alias[9]="HCLH_LOCKS"
alias[10]="TTAS_LOCKS"
alias[11]="SPINLOCK_LOCKS"
alias[12]="ARRAY_LOCKS"
alias[13]="CLH_LOCKS"
alias[14]="RW_LOCKS"
alias[15]="TICKET_LOCKS"
alias[16]="HTICKET_LOCKS"
alias[17]="MCS_RTM"
alias[18]="HCLH_RTM"
alias[19]="TTAS_RTM"
alias[20]="SPINLOCK_RTM"
alias[21]="ARRAY_RTM"
alias[22]="CLH_RTM"
alias[23]="RW_RTM"
alias[24]="TICKET_RTM"
alias[25]="HTICKET_RTM"


benchmarks[1]="bayes"
benchmarks[2]="genome"
benchmarks[3]="intruder"
benchmarks[4]="kmeans"
benchmarks[5]="labyrinth"
benchmarks[6]="ssca2"
benchmarks[7]="vacation"
benchmarks[8]="yada"

params[1]="-v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t"
params[2]="-g16384 -s64 -n16777216 -t"
params[3]="-a10 -l16 -n262144 -s1 -t"
params[4]="-m15 -n15 -t0.00001 -i inputs/random-n65536-d32-c16.txt -p"
params[5]="-i inputs/random-x512-y512-z7-n512.txt -t"
params[6]="-s20 -i0.5 -u0.1 -l3 -p3 -t"
params[7]="-n1 -q90 -u80 -r1048576 -t4194304 -c"
params[8]="-a20 -i inputs/ttimeu1000000.2 -t"

ext[1]=".rtm"
ext[2]=".seq"
ext[3]=""
ext[4]=""
ext[5]=""
ext[6]=".rtm"
ext[7]=".rtm"
ext[8]=".seq"
ext[9]=".seq"
ext[10]=".seq"
ext[11]=".seq"
ext[12]=".seq"
ext[13]=".seq"
ext[14]=".seq"
ext[15]=".seq"
ext[16]=".seq"
ext[17]=".rtm"
ext[18]=".rtm"
ext[19]=".rtm"
ext[20]=".rtm"
ext[21]=".rtm"
ext[22]=".rtm"
ext[23]=".rtm"
ext[24]=".rtm"
ext[25]=".rtm"



build[1]="rtm"
build[2]="seq"
build[3]="stm"
build[4]="stm"
build[5]="stm"
build[6]="rtm"
build[7]="rtm"
build[8]="seq"
build[9]="seq"
build[10]="seq"
build[11]="seq"
build[12]="seq"
build[13]="seq"
build[14]="seq"
build[15]="seq"
build[16]="seq"
build[17]="rtm"
build[18]="rtm"
build[19]="rtm"
build[20]="rtm"
build[21]="rtm"
build[22]="rtm"
build[23]="rtm"
build[24]="rtm"
build[25]="rtm"


for c in 17 18 19 20 21 22 23 24 25
do
    cd $workspace;
    bash config.sh ${config[$c]};
    bash build.sh ${build[$c]} ${alias[$c]};
    for b in 2 #3 4 5 6 7 8
    do
        for t in 4 #1 2 3 4 5 6 7 8
        do
#        for r in 1 2 3 4 5 6
#        do
#            sed -i "s/int tries = 4/int tries = $r/g" $workspace/lib/tm.h
            for a in 1 #2 3 4 5 #6 7 8 9 10
            do
                cd $workspace;
                cd ${benchmarks[$b]};
                echo "${config[$c]} | ${benchmarks[$b]} | retries $r | threads $t | attempt $a | ${alias[$c]}"
                ./../../IntelPerformanceCounterMonitorV2.5.1/pcm-tsx.x 1 -c > ../auto-results/${alias[$c]}-${benchmarks[$b]}-$t-$a.pcm &
                pid=$!
                ./../../power_gadget/power_gadget -e 100 > ../auto-results/${alias[$c]}-${benchmarks[$b]}-$t-$a.pow &
                pid2=$!
                ./${benchmarks[$b]}${ext[$c]} ${params[$b]}$t > ../auto-results/${alias[$c]}-${benchmarks[$b]}-$t-$a.data
                rc=$?
                kill -9 $pid
                kill -9 $pid2
                if [[ $rc != 0 ]] ; then
                    echo "Error within: ${alias[$c]} | ${benchmarks[$b]} | retries $r | threads $t | attempt $a" >> ../auto-results/error.out
                fi
            done
            cp $workspace/lib/tm.h.rtm $workspace/lib/tm.h    
#        done
        done
    done
done
