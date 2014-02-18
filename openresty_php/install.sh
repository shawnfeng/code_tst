# src url
url_openrestry="http://openresty.org/download/ngx_openresty-1.4.3.6.tar.gz"
url_php="http://cn2.php.net/distributions/php-5.5.9.tar.gz"

tar_openrestry=ngx_openresty-1.4.3.6
tar_php=php-5.5.9

# install path
ins_path="`pwd`/install"
#ins_path="/home/code/openresty_php"

init_pre()
{
    mkdir -p $ins_path

    # download
    wget $url_openrestry
    wget $url_php


    # uncomp
    tar xzvf $tar_openrestry.tar.gz
    tar xzvf $tar_php.tar.gz
}

# install openrestry
install_openrestry()
{
    cd $tar_openrestry
    ./configure --with-luajit --prefix=$ins_path/openrestry
    make -j 5
    make install
    cd ..
}

# install php
install_php()
{
    cd $tar_php
    ./configure --prefix=$ins_path/php --enable-fpm --with-openssl --with-pcre-regex --with-zlib  --with-gettext --enable-mbstring  --enable-zip --with-pear
    make -j 5
    make install
    cd ..
}


#init_pre
#install_openrestry
install_php