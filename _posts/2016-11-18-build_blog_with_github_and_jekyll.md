---
layout: post
title: 使用Nacos搭建SpringCloud集群
date: 2021-7-29
tags: [Nacos, GitHub, 教程, SpringCloud, Alibaba]
---

基于[Nacos](https://github.com/alibaba/nacos){:target="_blank"}搭建集群，希望对你有帮助。[`转载请标明出处，谢谢`]({{page.url}} "使用Nacos搭建集群")

### 概述 ###

> **`Nacos`** 是开源的服务发现与注册中心，一个更易于构建云原生应用的动态服务发现(Nacos Discovery)、服务配置(Nacos Config)和服务管理平台。 
> 
>服务发现和服务健康检查
>Nacos 使服务通过 DNS 或 HTTP 接口注册自己和发现其他服务变得简单。 Nacos 还提供服务的实时健康检查，以防止向不健康的主机或服务实例发送请求。
>
> 动态配置管理
> 动态配置服务允许您在所有环境中以集中和动态的方式管理所有服务的配置。 Nacos 无需在更新配置时重新部署应用程序和服务，这使得配置更改更加高效和敏捷。
>
> 动态 DNS 服务
> Nacos 支持加权路由，让您更容易在数据中心内的生产环境中实现中层负载均衡、灵活的路由策略、流量控制和简单的 DNS 解析服务。它可以帮助您轻松实现基于 DNS 的服务发现，并防止应用程序耦合 > 到特定于供应商的服务发现 API。
>
>服务和元数据管理
> Nacos 提供了一个易于使用的服务仪表板，帮助您管理您的服务元数据、配置、kubernetes DNS、服务健康和指标统计。

### 下载Nacos ###

1. 在[GitHub](https://github.com/alibaba/nacos/releases){:target="_blank"}上下载一个与项目匹配的 ***Assets*** ，Linux系统下载以 ***.tar*** 为后缀的包,Windows下载以 ***.zip*** 为后缀的包，如果想自己编译，配置，Alibaba也提供了Source源代码可供选择:


	![](/assets/build_alibaba_nacos/assets.png)
 
 
2. 解压后复制三份Nacos安装包，修改为nacos8849，nacos8850，nacos8851，搭建单机伪集群，分布式集群原理相同:

    > ```conf
    >  mv nacos nacos8849
    > ```


	![](/assets/build_alibaba_nacos/cluster.png)


3. 进入Nacos目录，如下图所示:

	![](/assets/build_alibaba_nacos/catalogue.png)
    
    
    修改conf\application.properties的配置，使用外置数据源  要使用mysql5.7+（包括）
    
    
    > ```conf
    > #使用外置mysql数据源
    > spring.datasource.platform=mysql
    > ### Count of DB:
    > db.num=1
    > ### Connect URL of DB:
    > db.url.0=jdbc:mysql://127.0.0.1:3306/nacos?characterEncoding=utf8&connectTimeout=1000&socketTimeout=3000&autoReconnect=true&useUnicode=true&useSSL=false&serverTimezone=UTC
    > db.user.0=root
    > db.password.0=root
    > ```


4. 将conf\cluster.conf.example改为cluster.conf,添加节点配置:


    > ```conf
    > ip:port
    > 本机IP地址:8849
    > 本机IP地址:8850
    > 本机IP地址:8851
    > ```


5. 创建mysql数据库,sql文件位置：conf\nacos-mysql.sql


6. 如果出现内存不足：修改启动脚本（bin\startup.sh）的jvm参数
    
    > ```conf
    > JAVA_OPT="${JAVA_OPT} ‐server ‐Xms512m ‐Xmx512m ‐Xmn256 ‐XX:MetaspaceSize=64m ‐XX:MaxMetaspaceSize=128m"
    > ```


    ![](/assets/build_alibaba_nacos/args.png)


7. 分别启动nacos8849，nacos8850，nacos8851，以下是启动nacos8849，其余相同
    
    
    > ```conf
    > cd nacos8849
    > bin/startup.sh 
    > ```


8. 访问Nacos管理界面，浏览器中输入 http://192.168.3.14:8849/nacos 默认的用户名密码是 nacos/nacos
    
    
    ![](/assets/build_alibaba_nacos/login.png)



### 安装配置Nginx ###


1. 安装Nginx前需要安装相关依赖:


    > ```conf
    >  yum -y install gcc
    >  yum install -y pcre pcre-devel
    >  yum install -y zlib zlib-devel
    >  yum install -y openssl openssl-devel
    >  wget http://nginx.org/download/nginx-1.9.9.tar.gz  
    >  tar -zxvf  nginx-1.9.9.tar.gz
    > ```
    
    
    切换到cd /usr/local/java/nginx-1.9.9/下面，执行三个命令：
    
    
    > ```conf
    > ./configure
    > make
    > make install
    > ```


2. 配置nginx的配置文件nginx.conf文件，在***http{}*** 内添加如下配置：
    
    
    > ```conf
    > upstream nacoscluster {
	>   server 127.0.0.1:8849;
	>   server 127.0.0.1:8850;
	>   server 127.0.0.1:8851;
    > }
    >
    > server {
	>   listen		8847;
	>   server_name	localhost;
	>
	> location /nacos/{
	>     	proxy_pass http://nacoscluster/nacos/;
	>   }
    > }
    > ```
    
    

3. 运行 nginx:


    > 切换目录到/usr/local/nginx/sbin下面，执行命令./nginx 启动nginx


4. 在浏览器中输入 [127.0.0.1](http://127.0.0.1){:target="_blank"} 进行本地预览


5. 成功启动的话，访问 http://本机ip地址:8847 访问nginx本地集群。





### 参考资料 ###

[Nacos文档](https://nacos.io/zh-cn/docs/what-is-nacos.html){:target="_blank"}  
[Nginx 下载](http://nginx.org/en/download.html){:target="_blank"}   
[Nacos 下载](https://github.com/alibaba/nacos/releases){:target="_blank"}  
[SpringCloudAlibaba 版本依赖关系](https://github.com/alibaba/spring-cloud-alibaba/wiki/版本说明){:target="_blank"}  

<br/>

> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)
