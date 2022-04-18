---
layout: post
title: linux部署springboot+vue项目
date: 2022-3-26
tags: [linux]
 
---

记录一下这几天部署项目的过程：

### 1.首先准备springboot项目

#### 1.1.切换一下配置文件

![image-20220326161922312](/assets/linux部署springboot+vue项目/image-20220326161922312.png)

#### 1.2.由于我们使用内置的tomcat服务器，所以直接打成jar包，在pom文件里面添加<packing>jar</packing>



![image-20220326162024411](/assets/linux部署springboot+vue项目/image-20220326162024411.png)

#### 1.3.点击clean执行完毕后点击package

![image-20220326162223388](/assets/linux部署springboot+vue项目/image-20220326162223388.png)

#### 1.4.执行完毕后就能看见target目录下有个jar包，这个就是我们需要的

![image-20220326162322996](/assets/linux部署springboot+vue项目/image-20220326162322996.png)

### 2.准备vue项目

##### 把vue项目里面的后端路由改成服务器ip:springboot运行端口

![image-20220326162447756](/assets/linux部署springboot+vue项目/image-20220326162447756.png)

#### 2.1.在vue目录里面执行npm install命令

![image-20220326162553644](/assets/linux部署springboot+vue项目/image-20220326162553644.png)

执行完毕

#### 2.2.执行npm run build

![image-20220326162703206](/assets/linux部署springboot+vue项目/image-20220326162703206.png)

执行完毕后，得到dist目录文件，这个就是我们要的

### 3.准备nginx服务器

![image-20220326162819101](/assets/linux部署springboot+vue项目/image-20220326162819101.png)

进入nginx目录的html文件夹，删除原有的index.html，把dist目录复制进来

![image-20220326163107465](/assets/linux部署springboot+vue项目/image-20220326163107465.png)

dist2， dist3还有renxi是我之前上传的，忽略掉

编辑nginx目录下conf目录里的nginx.config

![image-20220326163328570](/assets/linux部署springboot+vue项目/image-20220326163328570.png)

> location @router主要是为了解决前端界面刷新后404问题。

现在，就可以通过ip或者域名解析过的域名来访问项目啦。



> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)

