cd "/home/phak/star_net/sinicaphak_0709test/build"
cmake ..
make
echo "编译完成"

cd "/home/phak/star_net/sinicaphak_0709test/output"
cp ./main ./user_aaa
cp ./main ./user_bbb
echo "部署完成"
