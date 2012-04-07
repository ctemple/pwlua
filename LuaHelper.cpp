// LuaHelper.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "pwlua.h"
#include <stdio.h>

#pragma comment(lib,"lua5.1.lib")

class Test
{
public:
	Test()
	{
		printf("Test::Test\r\n");
	}

	~Test()
	{
		printf("Test::~Test\r\n");
	}

	virtual void print()
	{
		printf("Test::print\r\n");
	}

	void print2(int n121)
	{
		printf("Test::print2 %d\r\n",n121);
	}

	int print3()
	{
		return 120102;
	}
};

class TestB
{
public:
	TestB()
	{
		n = 1212;
	}
	TestB& printn()
	{
		printf("TestB::printn\r\n");
		return *this;
	}

	int n;
};

class Test2 : public Test,public TestB
{
public:
	Test2(TestB* v)
	{
		if(v != NULL)
			n = v->n;
	}

	virtual void print()
	{
		printf("Test2::print\r\n");
	}
};

int global_print()
{
	return 9999;
}

template<long NN> 
struct STest
{
	STest()
	{

	}
};


#define STRINGIZE(something) STRINGIZE_HELPER(something)
#define STRINGIZE_HELPER(something) #something
// #define ENFORCE(exp) MakeEnforcer((exp),__FILE__ "_" STRINGIZE(__LINE__)).Enforce()

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	pwlua::class_<Test>(L,"Test")
		.ctor()
		.method_fast<void, 1>("print",&Test::print)
		.method_fast<void, 2>("print2",&Test::print2)
		.method_slow<int>("print3",&Test::print3);
		
	pwlua::class_<TestB>(L,"TestB")
		.ctor()
		.method_fast<TestB&,0>("printn",&TestB::printn);

	

	pwlua::class_<Test2>(L,"Test2")
		.ctor<TestB*>()
		.inherit<Test>()
		.inherit<TestB>();	

	pwlua::method_slow<int>(L,"global_print",&global_print);
	pwlua::method_fast<int,1>(L,"global_print2",&global_print);

	lua_State* L2 = lua_newthread(L);
	
	int thread_ref = lua_ref(L,true);

	
	lua_pop(L,1);

	lua_gc(L,LUA_GCCOLLECT,0);

	if(luaL_dofile(L2,"../test.lua") != 0)
	{
		printf("%s",lua_tostring(L,-1));
	}

	{
		pwlua::temporary v(L2,"v");
		Test* pp = v.cast<Test*>();
		pwlua::temporary vp(L2,"print",-1);
		vp.invoke_nr<Test&>(*pp);
	}
	printf("type:%d\r\n",lua_type(L2,-1));
	{
		pwlua::reference vv(L2,"v");
		Test* pp = vv.cast<Test*>();
		pwlua::temporary vp(L2,"print",vv);
		vp.invoke_nr<Test&>(*pp);
	}
	printf("type:%d\r\n",lua_type(L2,-1));
	

	lua_gc(L,LUA_GCCOLLECT,0);
	lua_unref(L,thread_ref);
	lua_gc(L,LUA_GCCOLLECT,0);
	
	lua_close(L);
	return 0;
}

