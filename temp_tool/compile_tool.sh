cd "/home/phak/star_net/test0707/build"
cmake ..
make
echo "编译完成"

cd "/home/phak/star_net/test0707/output"
cp ./main ./user_aaa
cp ./main ./user_bbb
echo "部署完成"
