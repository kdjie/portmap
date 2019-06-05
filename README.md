portmap，可防CC的极简轻量级代理
==================================================
### 布署示意图：

```
攻击者   ------> x
正常用户 ------> v  portmap ------>  真实服务
攻击者   ------> x
```
### 防御类型：
- 空连接攻击
- DOS攻击
- DDOS/CC攻击

###### 原理：
通过对每个来源IP建立漏桶算法，初使化桶子的大小，并且控制桶子漏水的速度，只有当桶子有空间的时候，水才能装进去。桶子的大小，标识着用户一次性能建立的连接数量，对有些短连接的HTTP请求来说，设置一个比较合理的初使值，这是有必要的。漏水的速度，标志着用户可持续建立连接的能力。<br>
另外，如果恶意用户短时间内建立大量空连接，通过设置连接超时时间，和限制每个IP可发起的并发连接数，都可有效对真实服务器进行保护。

### 使用说明：
直接下载命令行工具，或者从github下载源码自行编译。

```
[kdjie@localhost portmap]$ ./portmap -v       
Usage: 
 Samples: 
     ./portmap -s 127.0.0.1:1982 -d 127.0.0.1:1983 --logterm --timeout 60 --limit_burst 10 --limit_speed 1/s --limit_connect 10 
 Options: 
     -s bindip:port 
     -d targetip:port 
     --logterm 
       open terminal log print, default close 
     --timeout seconds 
       set idle connection timeout, default 300s 
     --limit_burst 5 
       set max burst number for per source ip, default 10000 
     --limit_speed 1/m 
       set speed for per source ip, format: 1/s,10/m,100/h, default 10000/s 
     --limit_connect 1 
       set max connection for per source ip, default 100000
```
### 编译说明：
本工具在开源c++网络库libevwork的基础上进行开发。<br>
首先请下载libevwork，并按照说明进行编译。<br>
libevwork地址：https://github.com/kdjie/libevwork <br>

然后，请下载本工具的源码，进入工程目录，执行：<br>
make <br>
即可在工程目录生成portmap可执行文件。<br>

默认为生成静态可执行文件，如果编译报错，您可能需要安装以下库：<br>
yum install -y glibc-static libstdc++-static boost-static <br>

### 补充说明：
使用本工具做反向代理后，如果真实服务器有需要获取真实用户IP的场景，那么会造成取得的所有用户IP都是代理的IP，这是由于代理工作在TCP层造成的，目前还没有办法解决。<br>

在源代码bin目录下，作者已经上传了静态可执行文件版本，可以在任意linux64位平台上运行。