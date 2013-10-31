redis_run_path=/data/xce/redis_run
cfg_path=$redis_run_path/conf
log_path=$redis_run_path/log
bin_path=$redis_run_path/bin
port_begin=10020
redis_num=12

gen_config()
{

    echo "daemonize yes"
    echo "port $(($port_begin+$1))"
    echo "logfile $log_path/redis$1.log"


}

mkdir -p $cfg_path 
mkdir -p $log_path 

for idx in `seq $redis_num`; do
    tmp=$(($idx-1))
    gen_config $tmp > $cfg_path/redis$tmp.conf
done

for idx in `seq $redis_num`; do
    tmp=$(($idx-1))
    $bin_path/redis-server  $cfg_path/redis$tmp.conf
done

