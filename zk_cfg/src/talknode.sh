pr_node()
{
    path=$1
    echo $path
    ./talk $path | grep -v "^==========" |
    sed s/^.*server-//1 |sort -n |awk '{printf("cn=%d server-%s %s\n", cn++, $1, $2)} END {printf("count=%d\n", cn)}'
}

p_talkserver=/3g/ice/talk/bosh
p_endpoint=/3g/ice/talk/endpoint

pr_node $p_talkserver
pr_node $p_endpoint