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

#### 根据主键删除 ####
使用DELETE删除指定主键的文档：
> curl -XDELETE 'localhost:9200/product/book/1?pretty'
控制台输出：
```json
{
   "found": true,
   "_index": "product",
   "_type": "book",
   "_id": "1",
   "_version": 10,
   "result": "delete",
   "_shards": {
       "total": 2,
       "successful": 1,
       "failed": 0
   }
}
```
如果删除不存在的文档，found值为false。

#### 搜索文档 ####
ES提供了强大的搜索功能，搜索参数可以在url后面，也可以放到body中。使用GET方法：
> curl -G --data-urlencode 'q=message:炸酱面' 'localhost:9200/product/book/_search?pretty'
或者更通用的POST方法：
```json
curl -XPOST 'localhost:9200/product/book/_search?pretty' -H 'Content-Type:application/json' -d'
{
    "query": {
        "match": { "message": "介绍"}
    }
}'
```
> **注意：** 因为关键字里包含了中文，需要curl进行RUL编码，所以才使用了--data-urlencode 'q=message:炸酱面'，参数 "-G" 表示这是一个GET请求，
> 如果不加 "-G" ，curl默认发出POST请求，导致ES返回一个406不支持POST请求错误响应。

无论哪种查询，都会有类似如下的响应：
```json
{
    "took": 7,
    "timed_out": false,
    "_shards": {
        "total": 5,
        "successful": 5,
        "failed": 0
    },
    "hits": {
        "total": 2,
        "max_score": 0.789035,
        "hits": [
            {
                "_index": "product",
                "_type": "book",
                "_id": "2",
                "_score": 0.789035,
                "_source": {
                    "name": "100道菜",
                    "type": "case",
                    "postDate": "2021-9-9",
                    "message": "介绍100道菜"
                }
            },
            {
                "_index": "product",
                "_type": "book",
                "_id": "1",
                "_score": 0.78903234,
                "_source": {
                    "name": "100道菜",
                    "type": "case",
                    "postDate": "2021-9-9",
                    "message": "介绍炸酱面，卤煮等！"
                }
            }
        ]
    }
}
```

> hits包含了查询结果，在本例中，只有两条，Index是product，Type是book，主键是1和2，_score是搜索引擎概念，表示查询相关度，分数越高，表示此文档与关键字期望的结果匹配程度高。
> 除了全文搜索，还可以精度搜索，使用term进行精确搜索：

```json
curl -XGET 'localhost:9200/product/book/_search?pretty' -H 'Content-Type:application/json' -d'
{
    "query": {
        "term": { "type": "food"}
    }
}'
```
如果需要完成翻页功能，可以使用from和size：
```json
curl -XGET 'localhost:9200/product/book/_search?pretty' -H 'Content-Type:application/json' -d'
{
    "from": 0, "size": 5,
    "query": {
        "term": { "type": "food"}
    }
}'
```
如果需要知道查询的总数，则使用_count代替_search。
查询书中类型为菜谱的书的总数：
```json
curl -XGET 'localhost:9200/product/book/_count' -H 'Content-Type:application/json' -d'
{
    "query": {
        "term": { "type": "food"}
    }
}'
```
如果要联合条件查询，则可以使用must关键字：
```json
curl -XGET 'localhost:9200/product/book/_search?pretty' -H 'Content-Type:application/json' -d'
{
    "from": 0, "size": 5,
    "query": {
        "bool": {
            "must": {"match": { "type": "food"}},
            "must": {"term": { "message": "介绍"}}
        }
    }
}'
```


### 参考资料 ###

[SpringBoot2 精髓 李家智著]  
[Elasticsearch 官方文档](https://www.elastic.co/guide/cn/elasticsearch/guide/current/getting-started.html) 


<br/>

> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)
