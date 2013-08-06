pr_node()
{
    host=$1
    path=$2
    echo $path
    ./talk $host $path | grep -v "^==========" |
    sed s/^.*server-//1 |sort -n |awk '{printf("cn=%d server-%s %s\n", cn++, $1, $2)} END {printf("count=%d\n", cn)}'
}

p_talkserver=/3g/ice/talk/bosh
p_endpoint=/3g/ice/talk/endpoint

zkhost_online="UserZooKeeper1.d.xiaonei.com:2181,UserZooKeeper2.d.xiaonei.com:2181,UserZooKeeper3.d.xiaonei.com:2181,UserZooKeeper4.d.xiaonei.com:2181,UserZooKeeper5.d.xiaonei.com:2181"

zkhost_press="10.22.202.84:2181"

zkhost=$zkhost_press

pr_node $zkhost $p_talkserver
pr_node $zkhost $p_endpoint