# 作业名称：实现多人聊天

## 要求：

在命令行方式下实现16人以下的相互聊天

## 项目结构树
.
├── 项目文档                    项目设计文档
│   ├── 测试报告                测试报告
│   └── 设计报告                需求分析与软件设计
│       └── 设计图片            报告用到的部分图片
├── app                        main函数单独存放在这里
├── build                      cmake构建产物
├── inc                        头文件集中放在这里
├── output                     可执行文件输出在这里, config.json也要放在这里
├── src                        项目主要代码
│   └── tool                   工具类
└── test                       单元测试, 使用google test

## 使用

### 构建
```
mkdir build
cd ./build
cmake ..
make
```

默认不构建测试, 使用`cmake .. -DBUILD_TESTING=y && make`构建本项目测试, 需要翻墙下载google test框架

### 程序使用

1. 参照`./项目文档/设计报告/软件设计文档v1.doc`中6.3节 或 `inc/read_config.h`描述, 按需修改配置文件: `./output/config.json`
2. 运行`./output/main`
3. 程序接收键盘输入. 用户输入分两种模式: 输入模式与命令模式, 初始为输入模式. 
    a) 命令模式
        I                   进入到数据模式
        [0..9A..F]          选择/弃选聊天好友, 同样的代号再按下一次表示弃选 
        S                   发送聊天记录(按回车键后才生效, 不按s只按回车视为在聊天内容中输入回车键)
        Q                   退出程序
        其余字符输入         程序不处理
    b) 数据模式
        Esc                 进入到命令模式
        其余字符输入         输入聊天内容
4. 数据发送结构:
    - 使用udp socket通信
    - 广播:
        - 程序向`./inc/broadcast_thread.h/BROADCAST_IP`(默认为192.168.206.255)指定的网段广播ID包, 每5s发送一次, 向其他用户发送自身信息
        - 广播socket端口为10000, 在`./inc/main_thread.h/BROADCAST_PORT`处定义
        - ID包结构: `昵称名\tIP\t当前时间`
            - 当前时间为时间戳格式, 单位为秒
    - 聊天:
        - 程序向好友ip发送聊天内容包
        - 聊天socket端口为10001, 在`./inc/main_thread.h/CHAT_PORT`处定义
        - ID包结构: `昵称名\t己方IP\t好友名\t对方IP\t聊天内容`
5. 命令模式下输入`q`结束程序, 或者按两下`crtl c`

