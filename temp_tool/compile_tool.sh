cd "../build"
cmake ..
make
echo "编译完成"

cd "../output"
cp ./main ./user_aaa
cp ./main ./user_bbb
echo "部署完成"
