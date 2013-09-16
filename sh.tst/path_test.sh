pwd

echo $0

dirname $0

basename $0



run_path=`pwd`
dir_path=`dirname $0`
script_path=$run_path/$dir_path

echo $script_path

cd $script_path
cd ..
pwd

