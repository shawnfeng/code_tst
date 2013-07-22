
dir_root=/home/code/zkserv
dir_zkbin=/home/code/zookeeper-3.4.5/bin



dir_cfg=$dir_root/cfg

dir_serv0=$dir_root/serv0
dir_serv1=$dir_root/serv1
dir_serv2=$dir_root/serv2

dir_servs="$dir_serv0 $dir_serv1 $dir_serv2"
bindex=0
bclient_port=4180

echo_cfg()
{
    dir_serv=$1
    client_port=$2
    echo "tickTime=2000"
    echo "initLimit=5"
    echo "syncLimit=2"
    echo "dataDir=$dir_serv/data"
    echo "dataLogDir=$dir_serv/log"
    echo "clientPort=$client_port"
    echo "server.0=127.0.0.1:8880:7770"
    echo "server.1=127.0.0.1:8881:7771"
    echo "server.2=127.0.0.1:8882:7772"

}


for ds in $dir_servs; do

    mkdir -p $dir_cfg
    mkdir -p $ds/data
    mkdir -p $ds/log
    path_cfg=$dir_cfg/zoo$bindex.cfg
    echo_cfg $ds $bclient_port  >$path_cfg
    echo $bindex >$ds/data/myid

    sh $dir_zkbin/zkServer.sh start $path_cfg

    bclient_port=$(($bclient_port+1))
    bindex=$(($bindex+1))
done


