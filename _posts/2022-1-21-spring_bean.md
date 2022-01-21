---
layout: post
title: Spring底层概念
date: 2022-1-21
tags: [spring]
---

这几天有兴致想更新下博客，记录一下学习过程：

#### BeanDefination

BeanDefination表示bean定义，Spring根据BeanDefination来创建Bean对象，BeanDefination有多种属性用来描述Bean，BeanDefination是Spring中非常核心的概念。下面是BeanDefination中比较重要的几个属性：

##### 1.beanClass

表示一个bean的类型，比如UserService.class、OrderService.class，Spring在创建Bean的过程中会根据此属性来实例化对象。

##### 2.scope

表示一个bean的作用域，比如：

scope等于singleton，该bean就是一个单例Bean。

scope等于prototype，该bean就是一个原型bean。

##### 3.isLazy

表示一个bean是否需要懒加载，原型bean的isLazy属性不起作用，懒加载的单例bean，会在第一次getBean的时候生成该bean，非懒加载的单例bean，则会在Spring启动的时候直接生成好。

##### 4.dependsOn

表示一个bean在创建之前所依赖的其他bean，在一个bean创建之前，它所依赖的这些bean得先全部创建好。

##### 5.primary

表示一个bean是主bean，在Spring中一个类型可以有多个bean对象，在进行依赖注入时，如果根据类型找到多个bean，此时会判断这些bean中是否存在一个主bean，如果存在，则直接将这个bean注入给属性。

##### 6.initMethodName

表示一个bean的初始化方法，一个bean的生命周期过程中有一个步骤叫初始化，Spring会在这个步骤中调用bean的初始化方法，初始化方法逻辑由程序员自己控制，表示程序员可以自定义逻辑对bean进行加工。

> 像@Component，@Bean，<bean/>都会解析为BeanDefination

#### BeanFactory

BeanFactory是一种”Spring容器“，可以用来创建Bean，获取Bean。

##### 1.工作流程

BeanFactory将利用BeanDefination来生成Bean对象，BeanDefination相当于BeanFactory的原材料，Bean对象相当于BeanFactory生产出来的产品。

##### 2.BeanFactory的核心子接口和实现类

* ListableBeanFactory

* ConfigurableBeanFactory
* AutowireCapableBeanFactory
* AbstractBeanFactory
* DefaultListableBeanFactory

> DefaultListableBeanFactory是最重要的，它的功能有支持单例Bean，支持Bean别名，支持父子BeanFactory，支持Bean类型转换，支持Bean后置处理，支持FactoryBean，支持自动装配等。

#### 什么是Bean生命周期?

Bean生命周期描述的是Spring中一个Bean创建过程和销毁过程中所经历的步骤，其中Bean创建过程是重点。程序员可以利用Bean生命周期机制对Bean进行自定义加工。

##### 1.BeanDefinition

表示Bean定义，它定义了某个Bean的类型，Spring就是利用BeanDefinition来创建Bean的，比如需要利用BeanDefinition中beanClass属性确定Bean的类型，从而实例化出来对象。

##### 2.构造方法推断

一个Bean中可以有多个构造方法，此时就需要Spring来判断到底使用哪个构造方法，如果有无参构造方法，默认调用无参，如果没有无参构造，有一个有参构造，默认调用该有参构造，如果有多个有参构造，则根据类型找到bean再根据有参构造中的参数名称找到bean。还可以通过@Autowire注解配置默认构造方法。默认通过构造方法推断之后确定一个构造方法后，就可以利用构造方法实例化得到一个对象了。

##### 3.实例化
通过构造方法反射得到一个实例化对象，在Spring中，可以通过BeanPostProcessor机制对实例化进行干预。

##### 4.属性填充
实例化所得到的对象，是“不完整”的对象，“不完整”的意思是该对象中的某些属性还没有进行属性填充，也就是Spring还没有自动给某些属性赋值，属性填充就是我们通常说的自动注入、依赖注入。

##### 5.初始化
在一个对象的属性填充之后，Spring提供了初始化机制，程序员可以利用初始化机制对Bean进行自定义加工，比如可以利用InitializingBean接口来对Bean中的其他属性进行赋值，或对Bean中的某些属性进行校验。

##### 6.初始化后
初始化后是Bean创建生命周期中最后一个步骤，我们常说的AOP机制，就是在这个步骤中通过BeanPostProcessor机制实现的，初始化之后得到的对象才是真正的Bean对象。

#### @Autowired是什么?
Autowired表示某个属性是否需要进行依赖注入，可以写在属性和方法上。注解中的required属性默认为ture，表示如果没有对象可以注入给属性则抛异常。

Autowired加在某个属性上，Spring在进行Bean的生命周期过程中，在属性填充这一步，会基于实例化出来的对象，对该对象中加了Autowired的属性自动给属性赋值。Spring会先根据属性的类型去Spring容器中找出该类型所有的Bean对象，如果找出来多个，则再根据属性的名字从多个中再确定一个。如果required属性为true，并且根据属性信息找不到对象，则直接抛异常。

Autowired加在构造方法上时,Spring会在推断构造方法阶段,选择该构造方法来进行实例化，在反射调用构造方法之前,会先根据构造方法参数类型、参数名从Spring容器中找到Bean对象，当做构造方法入参。

#### @Resource是什么?
@Resource注解与@Autowired类似，也是用来进行依赖注入的，@Resource是Java层面所提供的注解，@Autowired是Spring所提供的注解，它们依赖注入的底层实现逻辑也不同。

@Resource注解中有一个name属性,针对name属性是否有值，@Resource的依赖注入底层流程是不同的。

@Reousrce如果name属性有值，那么Spring会直接根据所指定的name值去Spring容器找Bean对象，如果找到了则成功，如果没有找到，则报错。

如果@Resource中的name属性没有值，则:
1、先判断该属性名字在Spring容器中是否存在Bean对象。

2、如果存在，则成功找到Bean对象进行注入。

3、如果不存在，则根据属性类型去Spring容器找Bean对象，找到一个则进行注入。

#### Value是什么?
@Value注解和@Resource、@Autowired类似，也是用来对属性进行依赖注入的，只不过@Value是用来从Properties文件中来获取值的，并且Value可以解析SpEL(Spring表达式)。

@Value(${test}):spring会在配置文件中找到key为test的值注入给属性

@Value(#{testName}):spring会找到beanName为testName的bean注入给属性，没有找到则报错

##### FactoryBean是什么?
FactoryBean是Spring所提供的一种较灵活的创建Bean的方式，可以通过实现FactoryBean接口中的getObject()方法来返回一个对象，这个对象就是最终的Bean对象。

##### 1.FactoryBean接口中的方法
* Object getObject():返回的是Bean对象
* boolean isSingleton():返回的是否是单例Bean对象o
* Class getObjectType():返回的是Bean对象的类型

```java
@Component
public class TestFactoryBean implements FactoryBean {
    @Override
    // Bean对象
    public 0bject get0bject() throws Exception {
        return new User();
    }
    @Override
    //Bean对象的类型
    public Class<?> get0bjectType() {
        return User.class;
    }
    @Override
    //所定义的Bean是单例还是原型
    public boolean isSingleton() {
        return true;
    }
}

```

> FactoryBean的特殊点
> 上述代码。实际上对应了两个Bean对象:
> 1、bean对象为getObject方法所返回的User对象。
>
> 2、bean对象为ZhouyuFactoryBean类的实例对象。

FactoryBean对象本身也是一个Bean，同时它相当于一个小型工厂,可以生产出另外的Bean。

BeanFactory是一个Spring容器,是一个大型工厂,它可以生产出各种各样的Bean。

FactoryBean机制被广泛的应用在Spring内部和Spring与第三方框架或组件的整合过程中。



> [原始链接]({{page.url}}) 版权声明：自由转载-非商用-非衍生-保持署名 \| [Creative Commons BY-NC-ND 4.0](http://creativecommons.org/licenses/by-nc-nd/4.0/deed.zh)

