---
layout: post
title: docker部署pxc集群
date: 2022-4-18
tags: [docker]
 
---



### 1.首先准备拉取pxc镜像

#### 1.1.每台机器都需要执行

docker pull percona/percona-xtradb-cluster:5.7.21

改一下名字

docker tag percona/percona-xtradb-cluster:5.7.21 pxc

docker rmi percona/percona-xtradb-cluster:5.7.21



#### 1.2.初始化集群

在机器1上执行docker swarm init

执行完毕后会得到一条命令

**docker swarm join --token SWMTKN-1-... ip:port**

复制命令到其他机器上执行，显示

**This node joined a swarm as a worker.**

意味着其他机器成为机器1的worker节点



#### 1.3.创建docker 网络

docker network create -d overlay --attachable multinet



#### 1.4.创建主节点

docker run -d -p 7001:3306 -e MYSQL_ROOT_PASSWORD=123456 -e CLUSTER_NAME=PXC1 -e XTRABACKUP_PASSWORD=123456 \

-v v1:/var/lib/mysql --privileged --name=pn1 --net=multinet pxc



#### 1.5.创建从节点

docker run -d -p 7002:3306 -e MYSQL_ROOT_PASSWORD=123456 -e CLUSTER_NAME=PXC1 -e XTRABACKUP_PASSWORD=123456 \

-e CLUSTER_JOIN=pn1 -v v1:/var/lib/mysql --privileged --name=pn2 --net=multinet pxc



### 2.搭建mycat

#### 2.1.拉取mycat镜像

docker pull registry.cn-hangzhou.aliyuncs.com/ongo360/mycat:1.6.1

老样子，改一下名字

docker tag registry.cn-hangzhou.aliyuncs.com/ongo360/mycat:1.6.1 mycat-docker

docker rmi registry.cn-hangzhou.aliyuncs.com/ongo360/mycat:1.6.1



#### 2.2.运行mycat

docker run -d --name mycat  -p 8066:8066 -v mycat:/usr/local/mycat/conf -v mycat-logs:/usr/local/mycat/logs --net=multinet  mycat-docker



#### 2.3.配置mycat

执行docker volume ls 查看数据卷

![image-20220418202649504](/assets/docker部署pxc集群/image-20220418202649504.png)



执行docker volume inspect mycat 查看mycat数据卷位置

![image-20220418202803782](/assets/docker部署pxc集群/image-20220418202803782.png)

进入后编辑文件 schema.xml 

因为我们刚刚把mycat和pxc集群放在同一个虚拟内网multinet下，所以我们可以执行docker network ls 查看集群节点的内网地址

![image-20220418202226200](/assets/docker部署pxc集群/image-20220418202226200.png)

pn1的内网地址为172.18.0.2，pn2的内网地址为172.18.0.3

```xml
<?xml version="1.0"?>
<!DOCTYPE mycat:schema SYSTEM "schema.dtd">
<mycat:schema xmlns:mycat="http://io.mycat/">

	<schema name="TEST" checkSQLschema="false" sqlMaxLimit="100">

        <!-- table为真实数据库表 -->
		<table name="tb_users" primaryKey="id" type="global" dataNode="dn1,dn2" />

	</schema>

    <!-- database为真实数据库名称 -->
	<dataNode name="dn1" dataHost="localhost1" database="web" />
	<dataNode name="dn2" dataHost="localhost2" database="web" />
	<dataHost name="localhost1" maxCon="1000" minCon="10" balance="0"
			  writeType="0" dbType="mysql" dbDriver="native" switchType="1"  slaveThreshold="100">
		<heartbeat>select user()</heartbeat>
		<!-- pn1的url -->
		<writeHost host="hostM1" url="172.18.0.2:3306" user="root"
				   password="123456">
            <!-- pn2的url -->
			<readHost host="hostS2" url="172.18.0.3:3306" user="root" password="123456" />
		</writeHost>
	</dataHost>
	<dataHost name="localhost2" maxCon="1000" minCon="10" balance="0"
			  writeType="0" dbType="mysql" dbDriver="native" switchType="1"  slaveThreshold="100">
		<heartbeat>select user()</heartbeat>
		<writeHost host="hostM1" url="172.18.0.2:3306" user="root"
				   password="123456">
			<readHost host="hostS2" url="172.18.0.3:3306" user="root" password="123456" />
		</writeHost>
	</dataHost>
</mycat:schema>
```

还有server.xml

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!-- - - Licensed under the Apache License, Version 2.0 (the "License"); 
	- you may not use this file except in compliance with the License. - You 
	may obtain a copy of the License at - - http://www.apache.org/licenses/LICENSE-2.0 
	- - Unless required by applicable law or agreed to in writing, software - 
	distributed under the License is distributed on an "AS IS" BASIS, - WITHOUT 
	WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. - See the 
	License for the specific language governing permissions and - limitations 
	under the License. -->
<!DOCTYPE mycat:server SYSTEM "server.dtd">
<mycat:server xmlns:mycat="http://io.mycat/">
	<system>
	<property name="useSqlStat">0</property>  <!-- 1为开启实时统计、0为关闭 -->
	<property name="useGlobleTableCheck">0</property>  <!-- 1为开启全加班一致性检测、0为关闭 -->

		<property name="sequnceHandlerType">2</property>
      <!--  <property name="useCompression">1</property>--> <!--1为开启mysql压缩协议-->
        <!--  <property name="fakeMySQLVersion">5.6.20</property>--> <!--设置模拟的MySQL版本号-->
	<!-- <property name="processorBufferChunk">40960</property> -->
	<!-- 
	<property name="processors">1</property> 
	<property name="processorExecutor">32</property> 
	 -->
		<!--默认为type 0: DirectByteBufferPool | type 1 ByteBufferArena-->
		<property name="processorBufferPoolType">0</property>
		<!--默认是65535 64K 用于sql解析时最大文本长度 -->
		<!--<property name="maxStringLiteralLength">65535</property>-->
		<!--<property name="sequnceHandlerType">0</property>-->
		<!--<property name="backSocketNoDelay">1</property>-->
		<!--<property name="frontSocketNoDelay">1</property>-->
		<!--<property name="processorExecutor">16</property>-->
		<!--
			<property name="serverPort">8066</property> <property name="managerPort">9066</property> 
			<property name="idleTimeout">300000</property> <property name="bindIp">0.0.0.0</property> 
			<property name="frontWriteQueueSize">4096</property> <property name="processors">32</property> -->
		<!--分布式事务开关，0为不过滤分布式事务，1为过滤分布式事务（如果分布式事务内只涉及全局表，则不过滤），2为不过滤分布式事务,但是记录分布式事务日志-->
		<property name="handleDistributedTransactions">0</property>
		
			<!--
			off heap for merge/order/group/limit      1开启   0关闭
		-->
		<property name="useOffHeapForMerge">1</property>

		<!--
			单位为m
		-->
		<property name="memoryPageSize">1m</property>

		<!--
			单位为k
		-->
		<property name="spillsFileBufferSize">1k</property>

		<property name="useStreamOutput">0</property>

		<!--
			单位为m
		-->
		<property name="systemReserveMemorySize">384m</property>


		<!--是否采用zookeeper协调切换  -->
		<property name="useZKSwitch">true</property>


	</system>
	
	<!-- 全局SQL防火墙设置 -->
	<!-- 
	<firewall> 
	   <whitehost>
	      <host host="127.0.0.1" user="mycat"/>
	      <host host="127.0.0.2" user="mycat"/>
	   </whitehost>
       <blacklist check="false">
       </blacklist>
	</firewall>
	-->
	
	<user name="root">
		<property name="password">123456</property>
        <!-- 虚拟数据库 -->
		<property name="schemas">TEST</property>
		
		<!-- 表级 DML 权限设置 -->
		<!-- 		
		<privileges check="false">
			<schema name="TESTDB" dml="0110" >
				<table name="tb01" dml="0000"></table>
				<table name="tb02" dml="1111"></table>
			</schema>
		</privileges>		
		 -->
	</user>

<!--	<user name="user">-->
<!--		<property name="password">user</property>-->
<!--		<property name="schemas">TESTDB</property>-->
<!--		<property name="readOnly">true</property>-->
<!--	</user>-->

</mycat:server>

```

#### 2.4.连接mycat

用navicat连接mycat

![image-20220418204008910](/assets/docker部署pxc集群/image-20220418204008910.png)



### 3.可能出现的错误

#### 3.1.直接打开虚拟库中的表出现，在不切换到具体的数据库下，使用[数据库名].[表名]的方式会抛出下面的错误

![image-20220418204548160](/assets/docker部署pxc集群/image-20220418204548160.png)

使用正常的查询语句就不会有异常

![image-20220418204817347](/assets/docker部署pxc集群/image-20220418204817347.png)

如何修改？

**将schema.xml的 checkSQLschema=“false”,改为true重启mycat即可**

当该值为true时，例如我们执行语句select * from TEST.tb_users 。mycat会把语句修改为 select * from tb_users去掉TEST。

#### 3.2.stop节点后出现主节点无法正常运行的情况
进入主节点的数据卷中，编辑grastate.dat这个文件

**将safe_to_bootstrap: 0 改成safe_to_bootstrap: 1**

就可以运行主节点了！


> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)

