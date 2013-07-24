rm qs dm qsd

gcc qs.c  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o qs

gcc qsd.c  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o qsd

gcc dm.c  -I/home/code/rp/zookeeper/include  -I/home/code/rp/zookeeper/include/zookeeper -L/home/code/rp/zookeeper/lib -lzookeeper_mt -Wl,-rpath,/home/code/rp/zookeeper/lib -o dm