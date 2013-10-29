rm -f qs dm qsd talk ephemeral_node

gcc qs.c  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o qs

gcc qsd.c  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o qsd

gcc dm.c  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o dm

g++ talk.cc  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o talk

g++  ephemeral_node.cc  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o ephemeral_node


g++ online_config.cc  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o online_config