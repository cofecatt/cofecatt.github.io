---
layout: post
title: Elasticsearch教程与实例
date: 2021-9-9
tags: [Elasticsearch]
---

### 概述 ###
Elasticsearch简称ES，是一个全文搜索服务器，也可以作为NoSQL数据库，存储任意格式的文档和数据，同时，也可以做大数据的分析，是一个跨界开源产品。ES有如下特点：
> * 全文搜索引擎，ES是建立在Lucene上的开源搜索引擎可以用来进行全文搜索地理信息搜索。Wikipedia、GitHub、 StackOverFlow等网站均使用 ES.

> * 文档存储和查询，可以像 NoSQL 那样存储任意格式文档，并能根据条件查询文档。

> * 大数据分析，ES号称能准确实时地进行大数据分析，数据量从TB到PB,国内外很多大公司都用ES做大数据分析。

> * ES提供了REST API，用来简化对 ES 的操作。因此可以使用任何语言的客户端，同时也提供Java API,Spring Boot也对 REST API 进行了封装，简化了开发。

> * ES常常配合传统数据库一起使用，ES用来负责大数据的查询、搜索、统计分析。


> ![](/assets/alfred_workflow/06.jpg)
>
> * `Effective IP`: 查询本机和外网IP地址，解析任意URL和域名的IP地址，同时进行归属地和运营商查询，使用 *python* 实现 [点此下载](https://raw.githubusercontent.com/stidio/Alfred-Workflow/master/Effective%20IP.alfredworkflow){:target="_blank"}
>
> ![](/assets/alfred_workflow/10.jpg)  
> ![](/assets/alfred_workflow/11.jpg)  
> ![](/assets/alfred_workflow/12.jpg)  
> ![](/assets/alfred_workflow/13.jpg)
>

本文源代码地址：[https://github.com/cofecatt/cofecatt.github.io/blob/main/_posts/2021-9-9-elasticsearch.md](https://github.com/cofecatt/cofecatt.github.io/blob/main/_posts/2021-9-9-elasticsearch.md)，如果喜欢请[Star!](https://github.com/cofecatt/cofecatt.github.io)，谢谢!



ES有一些基本概念，掌握这些基本概念对理解ES有很大帮助。
> * Index， index是文档(Document)的集合，Index下面包含了Type，用于对Document进一步分类。可以理解为ES中的Index相当于数据库，而Type相当于数据库中的表，ES中可以轻易的联合Index和Type来搜索数据，数据库却不能。

> * Type，用来进一步组织Document，一个Index下可以有多个Type，比如用户信息是一个Type，用户的支付记录是一个Type。

> * Document，文档是ES能够存储和搜索的基本信息，类似数循厍衣仃效话，Document为JSON格式，文档属于 Type。

> * Node（节点)，节点是集群里的一台ES Server，用于文档的存储和查询。应用可以只有一个节点，也可以由上百个节点组成集群来存储和搜索数据。每个节点都有一个节点名字,以及所属集群的名字。

> * 集群,同样集群名的节点将组合为ES集群，用来联合完成数据的存储和搜索。默认的集群名字是elasticsearch。
 
> * 分区(Shards)和复制(Replicas)，每个Index理论上都可以包含大量的数据，超过了单个节点的存储限制，而且单个节点处理那么大的数据，将明显限制存储和搜索
性能。为了解决这个问题，ES 会进一步将Index在物理上细分为多个分区，且A区分区会按照配置复制到多个节点，Index 的分区称为主分区，复制的分区称为复制分区。
这样的好处是既保证数据不会丢失，又提高了查询的性能。每个分区是一个独立的工作单元，可以完成存储和搜索功能，每个分区能存储最多2147483519个文档。



### 使用REST访问Elasticsearch ###

#### 1. 添加文档 ####
使用PUT添加文档，采用curl，在命令行输入：
```json
curl -XPOST 'localhost:9200/product/book/1?pretty' -H 'Content-Type:application/json' -d'
{
    "name": "100道菜",
    "type": "case",
    "postDate": "2021-9-9",
    "message": "介绍100道菜"
}'
```

product表示Index，book表示Type，数字1是文档的主键，主键可以是任意形式，如果未指定主键，ES会自动生成一个唯一主键，pretty是可选的，ES输出的时候会格式化输出的结果，更加美观。
> *  如果不熟悉curl，可以使用postman工具代替。

postDate的格式是ES默认的日期格式之一，为 *yyyy-MM-dd* ，ES还默认了多种格式为日期格式，ES会自动认为这些类型为日期格式，一下三种格式数据ES会处理成日期类型。

> *  *yyyy-MM-dd* ，如2021-9-9
> *  *yyyy-MM-dd'T'HH:mm:ss* ，如2021-9-9T13:14:21
> *  *yyyy-MM-dd'T'HH:mm:ss.SSS* ，如2021-9-9T13:14:21.398

最后操作的响应是:
```json
{
    "_index": "product",
    "_type": "book",
    "_id": "1",
    "result": "created",
    "_shards": {
        "total": 2,
        "successful": 1,
        "failed": 0
    },
    "created": true
}
```
> * 字段_id表示该文档的主键，如果在添加文档的时候未指定主键，系统默认生成一个唯一主键。
> * _shards表示分区信息，total为2表示有两个分区（包括主分区），successful为1表示成功复制了一份。
> * _version代表了文档的版本号，每一次修改都会递增，注意ES并不会存储文档修改的各个版本。

#### 根据主键查询 ####
> 输入curl -XGET 'localhost:9200/product/book/1?pretty'

控制台输出：
```json
{
    "_index": "product",
    "_type": "book",
    "_id": "1",
    "_version": 1,
    "found": true,
    "_source": {
        "name": "100道菜",
        "type": "case",
        "postDate": "2021-9-9",
        "message": "介绍100道菜"
    }
}

```
> * source表示查询的文档，这正是我们存入的文档。如果你只想看到source部分，可以加一个_source: curl -XGET 'localhost:9200/product/book/1/_source?pretty'

#### 根据主键更新 ####
根据主键更新和新增主键一样，需要指定主键然后更新整个文档。
```json
curl -XPUT 'localhost:9200/product/book/1?pretty' -H 'Content-Type:application/json' -d'
{
    "name": "108道菜",
    "type": "case",
    "postDate": "2021-9-9",
    "message": "介绍100道菜"
}'
```

输出：

```json
{
    "_index": "product",
    "_type": "book",
    "_id": "1",
    "_version": 2,
    "result": "updated",
    "_shards": {
        "total": 2,
        "successful": 1,
        "failed": 0
    },
    "created": false
}
```

修改文档同添加文档类似，不同的地方是_version递增了，result的值是updated，表示更新成功。如果只是想局部更新，可以采用POST，使用_update，比如只更新message字段：
```json
curl -XPOST 'localhost:9200/product/book/1/_update?pretty' -H 'Content-Type:application/json' -d'
{
    "doc": {
        "message": "介绍炸酱面，卤煮等！"
    }
}'
```


字段doc包含了要更新的片段，如果并发修改文档，可以使用version字段实现乐观锁，如果修改的文档的version和传入的version不一致，则修改失败。
```json
curl -XPOST 'localhost:9200/product/book/1/_update?pretty&version=2' -H 'Content-Type:application/json' -d'
{
    "doc": {
        "message": "介绍炸酱面，卤煮等！"
    }
}'
```
如果当前文档版本号不是2的话，则修改失败，控制台抛出如下错误：
```json
{
    "error":{
        "root_cause":[
            "type": "version_conflict_engine_exception",
            ...
        ],
        "type": "version_conflict_engine_exception",
        ...
    },
    "status": 409
}
```
可以看到status为409，type字段为version_conflict_engine_exception，表示版本冲突，更新失败。


？？
2. 点击Workflows按钮，然后点击最下面的 **+** 按钮，创建一个Blank Workflow，按照提示填入信息:

    ![](/assets/alfred_workflow/02.jpg)

    > **Bundle Id** 作为该Workflow的标识为必填内容，如果不填或与其他重复，有可能造成其不能正常运行

### Workflow - CDto ###

使用Terminal的一般步骤大概是运行Terminal，然后一路cd到目标文件夹后开始使用；虽然Finder有cd to插件，但也需要你一路点到指定文件夹后，才能调起来；虽然Alfred的Right Arrow按键里面有Open Terminal Here操作，但排在太后面了，打开的操作路径至少需要：Right Arrow -> 输入o -> [Command + 3]三步才能完成:

![](/assets/alfred_workflow/03.jpg)

作为一个需要频繁和Terminal交互的码农这完全不能忍，下面我们就利用Workflow做个一步到位的CDto神器

1. 在Alfred Workflows的工作区点右键，选择菜单[Inputs -> File Filter]，并按下图设置好，其他两个选项卡使用默认设置即可:

    ![](/assets/alfred_workflow/04.jpg)

2. 在刚才插入的[File Filter]上点击右键，选择菜单[Insert After -> Actions -> Run Script]，并按照下图设置好，最下面的Escaping表示对指定字符进行转义，比如说:/Users/$a1，如果不对$转义，那外部会把$a1一起当做一个变量，而这个变量未定义也就是为空，传递进来的参数最终变成:/Users/，[点此查看代码](https://github.com/stidio/Alfred-Workflow/blob/master/CDto/cdto.bash){:target="_blank"}:

    ![](/assets/alfred_workflow/05.jpg)

### Workflow - Effective IP ###

现在我们使用Python来做个更复杂的例子，[点此查看源码](https://github.com/stidio/Alfred-Workflow/blob/master/Effective%20IP/effectiveip.py){:target="_blank"}，具体分析见下图:

![](/assets/alfred_workflow/07.jpg)

我们基于[Full-featured python library for writing Alfred workflows](https://github.com/deanishe/alfred-workflow/){:target="_blank"}进行开发，具体的内容请参考前面的内容和[官方教程](http://www.deanishe.net/alfred-workflow/tutorial_1.html){:target="_blank"}, 这里我只对两个设置界面进行必要的解释：

1. 主设置界面

    ![](/assets/alfred_workflow/08.jpg)

    > 1. 直接输入ip无参形式是查询本机的本地和公网地址，有参形式是进行DNS解析，因此参数是可选的，需要设置为：[Argument Optional]
    > 2. 点击Run Behaviour按钮，进行运行行为设置

2. 运行行为设置

    ![](/assets/alfred_workflow/09.jpg)

    > 1. 如果输入发生变化，我们肯定是希望得到之后的结果，因此我们需要即时结束掉之前的查询
    > 2. 在输入过程中不进行查询，Alfred通过最后一个字符输入延迟来判断输入结束后才进行查询

### 其他事项 ###

![](/assets/alfred_workflow/14.jpg)

> 1. 左边列表区域里点右键选择[Open in Finder]可以打开该Workflow的目录进行文件查看和编辑
> 2. 点此可以调出调试窗口，查看调试信息

### 参考资料 ###

[SpringBoot2 精髓 李家智著]  
[Alfred workflow 开发指南](http://myg0u.com/python/2015/05/23/tutorial-alfred-workflow.html){:target="_blank"}  
[JavaScript for OS X Automation by Example](http://developer.telerik.com/featured/javascript-os-x-automation-example/){:target="_blank"}  
[Full-featured python library for writing Alfred workflows](http://www.deanishe.net/alfred-workflow/){:target="_blank"}

<br/>

> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)
