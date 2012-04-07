轻量级lua-c++绑定库
*泛型实现
*支持多继承
*继承体系间的对象能自动安全转换(基类子对象的偏移)

***为保证性能,导致了一个不易用的地方
***同原型的函数会保存到同一个变量上，因此加了一个常量以区别
***如果要注册一个以上同原型的函数，必须使用method2方法
***ps:__LINE__不行

lua_State* L = luaL_newstate();
luaL_openlibs(L);

pwlua::class_<Test>(L,"Test")
	.ctor()
	.method2<void, 1>("print",&Test::print)
	.method2<void, 2>("print2",&Test::print2);
	
pwlua::class_<TestB>(L,"TestB")
	.ctor()
	.method<TestB&>("printn",&TestB::printn);



pwlua::class_<Test2>(L,"Test2")
	.ctor<TestB*>()
	.inherit<Test>()
	.inherit<TestB>();	

pwlua::method<int>(L,&global_print,"global_print");
pwlua::method2<int,1>(L,&global_print,"global_print");


pwlua::reference vv(L2,"v");
Test* pp = vv.cast<Test*>();
pwlua::temporary vp(L2,"print",vv);
vp.invoke_nr<Test&>(*pp);