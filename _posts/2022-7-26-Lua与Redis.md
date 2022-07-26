---
layout: post
title: Lua和Redis类型转换
date: 2022-7-26
tags: [redis, Lua]
 
---



## Lua 数据类型和 Redis 数据类型转换

当 Lua 使用 call() 或 pcall() 调用 Redis 命令时，Redis 返回值将转换为 Lua 数据类型。同样，在调用 Redis 命令和 Lua 脚本返回值时，Lua 数据类型会转换为 Redis 协议，以便脚本可以控制 EVAL 将返回给客户端的内容。

这种数据类型之间的转换是这样设计的，如果将 Redis 类型转换为 Lua 类型，然后将结果转换回 Redis 类型，结果与初始值相同。

换句话说，Lua 和 Redis 类型之间存在一对一的转换。下表显示了所有转换规则：

**Redis 到 Lua 的转换表。** 

* Redis 整数返回值 -> Lua number
* Redis 批量返回值 -> Lua 字符串
* Redis 多批量返回值 -> Lua 表（可能嵌套了其他 Redis 数据类型）
* Redis 状态返回值 -> Lua 表，其中包含一个包含状态的 ok 字段
* Redis 错误返回值 -> Lua 表，其中包含包含错误的单个 err 字段
* Redis Nil 批量返回值和 Nil 多批量返回值 -> Lua false 布尔类型



**Lua 到 Redis 的转换表。**

* Lua number -> Redis integer 返回值（数字转换为整数）
* Lua 字符串 -> Redis 批量返回值
* Lua 表（数组）-> Redis 多批量返回值（如果有，则截断为 Lua 数组中第一个 nil）
* 带有单个 ok 字段的 Lua 表 -> Redis 状态返回值
* 带有单个 err 字段的 Lua 表 -> Redis 错误返回值
* Lua boolean false -> Redis Nil 批量返回值。



还有一个额外的 Lua 到 Redis 的转换规则，它没有对应的 Redis 到 Lua 的转换规则：

Lua boolean true -> Redis 整数返回值，值为 1。
最后，需要注意三个重要规则：

Lua 有一个单一的数字类型，Lua numbers。整数和浮点数之间没有区别。所以我们总是将 Lua 数字转换为整数返回值，如果有的话，删除数字的小数部分。如果你想从 Lua 返回一个浮点数，你应该将它作为一个字符串返回，就像 Redis 本身所做的一样（例如参见 ZSCORE 命令）。
没有简单的方法在 Lua 数组中包含 nil，这是 Lua 表语义的结果，因此当 Redis 将 Lua 数组转换为 Redis 协议时，如果遇到 nil，则停止转换。
当 Lua 表包含键（及其值）时，转换后的 Redis 返回值将不包含它们。



> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)

