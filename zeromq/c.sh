gcc server.c  -I /home/shawn/install/zeromq/include -L /home/shawn/install/zeromq/lib -lzmq  -Wl,-rpath,/home/shawn/install/zeromq/lib -o server_c
gcc client.c  -I /home/shawn/install/zeromq/include -L /home/shawn/install/zeromq/lib -lzmq  -Wl,-rpath,/home/shawn/install/zeromq/lib -o client_c


g++ server.cxx  -I /home/shawn/install/zeromq/include -L /home/shawn/install/zeromq/lib -lzmq  -Wl,-rpath,/home/shawn/install/zeromq/lib -o server_cxx