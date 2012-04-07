轻量级lua-c++绑定库
*泛型实现
*支持多继承
*继承体系间的对象能自动安全转换(基类子对象的偏移)

***method_fast为保证性能,导致了一个不易用的地方
***同原型的函数会保存到同一个变量上，因此加了一个常量以区别
***如果要注册一个以上同原型的函数，必须使用method2方法，并传到不同的常量N

***method绑定需要创建一个userdata，并在其元表的__call中调用原始方法,比fast版本稍慢

***temporary引用lua栈对象,在c++中必须该类只能为栈变量

pwlua::class_<Test>(L,"Test")
	.ctor()
	.method_fast<void, 1>("print",&Test::print)
	.method_fast<void, 2>("print2",&Test::print2)
	.method<int>("print3",&Test::print3);
	
pwlua::class_<TestB>(L,"TestB")
	.ctor()
	.method_fast<TestB&,0>("printn",&TestB::printn)
	.member<int>("n",&TestB::n);



pwlua::class_<Test2>(L,"Test2")
	.ctor<TestB*>()
	.inherit<Test>()
	.inherit<TestB>()
	.member<TestB*>("pp",&Test2::pp);	

pwlua::method<int>(L,"global_print",&global_print);
pwlua::method_fast<int,1>(L,"global_print2",&global_print);