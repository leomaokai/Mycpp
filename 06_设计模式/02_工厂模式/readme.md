简单工厂:
* 把对象的创建封装再一个接口函数里面,通过传入一个不同的标识,返回创建的对象
* 提供创建对象实例的接口函数不闭合
工厂方法:
* 给工厂创建一个基类
* 具体产品的工厂负责创建对应的产品
* 可以做到不同的产品在不同的工厂里面创建
* 能够对现有工厂以及产品的修改关闭
抽象工厂:
* 把有关联关系的属于一个产品族的所有产品的接口函数放到一个抽象工厂里
* 具体工厂应该负责创建该产品族里面的所有产品