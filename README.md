# protobuf

1.确定centos7上已经安装了下面的软件，或者直接用yum进行更新

autoconf
automake
libtool
curl (used to download gmock)
make
g++
unzip

2.下载源码包，解压，编译安装

地址：https://github.com/google/protobuf/releases

选择Source code (tar.gz)下载

tar -zxvf protobuf-3.1.0.tar.gz -C /usr/local/
cd protobuf-3.1.0/
./autogen.sh
# 指定安装路径
./configure --prefix=/usr/local/protobuf
#编译
make
# 测试，这一步很耗时间
make check
make install

3.编写proto文件

4.执行build.sh脚本，生成相关c文件

5.业务逻辑编写
