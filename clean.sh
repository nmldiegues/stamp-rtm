rm tokill;
ps aux | grep 'Intel' | grep -v 'grep' | cut -d ' ' -f 7 > tokill;
for pid in `cat tokill`
do 
    if [[ $pid != "0.0" ]] ; then 
        sudo kill -9 $pid;
        echo "killed $pid";
    fi
done
rm tokill;

ps aux | grep 'Intel' | grep -v 'grep' | cut -d ' ' -f 6 > tokill;
for pid in `cat tokill`
do
    if [[ $pid != "0.0" ]] ; then
        sudo kill -9 $pid;
        echo "killed $pid";
    fi
done
rm tokill;

