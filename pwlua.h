/********************************************************************
	created:	2012/04/07
	created:	7:4:2012   10:03
	filename: 	e:\tmp\LuaHelper\pwlua.h
	file path:	e:\tmp\LuaHelper
	file base:	pwlua
	file ext:	h
	author:		cbh
	
	purpose:	
*********************************************************************/
#ifndef _pw_lua_
#define _pw_lua_

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <cassert>
#include <string>

namespace pwlua
{
#ifdef _WIN32
	typedef __int64 int64;
	typedef __int32 int32;
#else
	typedef long long int64;
	typedef int int32;
#endif

	template<class T> class class_name
	{
	public:
		static int meta;
		static char name[128];
	};

	template<class T> int	class_name<T>::meta = LUA_NOREF;
	template<class T> char	class_name<T>::name[128] = "";


	// ---------------------------------------------------------------------------------------------------------

	static const char* _class_name = "name";
	static const char* _base_classes = "baseclasses";
	static const char* _cast = "cast";

	// ---------------------------------------------------------------------------------------------------------

	namespace _detail
	{

		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		template<class T> struct stack_helper
		{
			/*
			static void push(lua_State* L,const T& val)
			{
				assert(class_name<T>::meta != LUA_NOREF && "class nofound");
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_newuserdata(L,sizeof(object<T>::proxy)));
				_proxy->gc = false;
				_proxy->offset = 0;
				_proxy->obj = const_cast<T*>(&val);
				_proxy->meta = class_name<T>::meta;
				lua_getref(L,class_name<T>::meta);
				lua_setmetatable(L,-2);
			}

			static T& from(lua_State* L,int index)
			{
				assert(class_name<T>::meta != LUA_NOREF && "class nofound");
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_touserdata(L,index));
				
				return *_proxy->obj;
			}
			*/
		};

		// ***************************************************************************

		template<class T> struct stack_helper<T*>
		{
			static void push(lua_State* L,T* val)
			{
				assert(class_name<T>::meta != LUA_NOREF && "class nofound");
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_newuserdata(L,sizeof(object<T>::proxy)));
				_proxy->gc = true;
				_proxy->offset = 0;
				_proxy->obj = val;
				_proxy->meta = class_name<T>::meta;
				lua_getref(L,class_name<T>::meta);
				lua_setmetatable(L,-2);
			}

			static T* cast(lua_State* L,int index)
			{
				assert(lua_isuserdata(L,index) || lua_isnil(L,index));
				assert(class_name<T>::meta != LUA_NOREF && "class nofound");
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_touserdata(L,index));
				if(_proxy == NULL)
					return NULL;
				
				if(_proxy->meta == class_name<T>::meta)
				{
					return (T*)_proxy->obj;
				}
				else
				{
					lua_getref(L,_proxy->meta);
					lua_getfield(L,-1,_cast);
					lua_remove(L,-2);
					lua_pushlightuserdata(L,_proxy->obj);
					lua_pushinteger(L,(LUA_INTEGER)&class_name<T>::name[0]);
					lua_pcall(L,2,1,0);

					T* result = (T*)lua_touserdata(L,-1);
					lua_pop(L,1);
					return result;
				}
			}
		};

		// ***************************************************************************

		template<class T> struct stack_helper<T&>
		{
			static void push(lua_State* L,T& val)
			{
				assert(class_name<T>::meta != LUA_NOREF && "class nofound");
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_newuserdata(L,sizeof(object<T>::proxy)));
				_proxy->gc = false;
				_proxy->offset = 0;
				_proxy->obj = &val;
				_proxy->meta = class_name<T>::meta;
				lua_getref(L,class_name<T>::meta);
				lua_setmetatable(L,-2);
			}
			
			static T& cast(lua_State* L,int index)
			{
				assert(lua_isuserdata(L,index) || lua_isnil(L,index));
				assert(class_name<T>::meta != LUA_NOREF && "class nofound");
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_touserdata(L,index));
				if(_proxy == NULL)
					return NULL;

				if(_proxy->meta == class_name<T>::meta)
				{
					return (T*)_proxy->obj;
				}
				else
				{
					lua_getref(L,_proxy->meta);
					lua_getfield(L,-1,_cast);
					lua_remove(L,-2);
					lua_pushlightuserdata(L,_proxy->obj);
					lua_pushinteger(L,(LUA_INTEGER)&class_name<T>::name[0]);
					lua_pcall(L,2,1,0);

					T& result = *((T*)lua_touserdata(L,-1));
					lua_pop(L,1);
					return *result;
				}
			}
		};

		// ***************************************************************************

		template<> struct stack_helper<char>
		{		
			static void push(lua_State* L,const char& val)
			{
				lua_pushinteger(L,val);
			}

			static char cast(lua_State* L,int index)
			{
				assert(lua_isnumber(L,index));
				return lua_tointeger(L,index);
			}
		};

		// ***************************************************************************

		template<> struct stack_helper<short>
		{		
			static void push(lua_State* L,const short& val)
			{
				lua_pushinteger(L,val);
			}

			static short cast(lua_State* L,short index)
			{
				assert(lua_isnumber(L,index));
				return lua_tointeger(L,index);
			}
		};

		// ***************************************************************************

		template<> struct stack_helper<int32>
		{		
			static void push(lua_State* L,const int32& val)
			{
				lua_pushinteger(L,val);
			}

			static int32 cast(lua_State* L,int32 index)
			{
				assert(lua_isnumber(L,index));
				return lua_tointeger(L,index);
			}
		};

		// ***************************************************************************

		template<> struct stack_helper<int64>
		{		
			static void push(lua_State* L,const int64& val)
			{
				lua_pushnumber(L,(LUA_NUMBER)val);
			}

			static int64 cast(lua_State* L,int index)
			{
				assert(lua_isnumber(L,index));
				return (int64)lua_tonumber(L,index);
			}
		};

		// ***************************************************************************

		template<> struct stack_helper<double>
		{		
			static void push(lua_State* L,const double& val)
			{
				lua_pushnumber(L,val);
			}

			static double cast(lua_State* L,int index)
			{
				assert(lua_isnumber(L,index));
				return lua_tonumber(L,index);
			}
		};

		// ***************************************************************************
		
		template<> struct stack_helper<const char*>
		{		
			static void push(lua_State* L,const char* val)
			{
				lua_pushstring(L,val);
			}

			static const char* cast(lua_State* L,int index)
			{
				assert(lua_isstring(L,index) || lua_isnil(L,index));
				return lua_tostring(L,index);
			}
		};

		// ***************************************************************************

		template<> struct stack_helper<std::string>
		{		
			static void push(lua_State* L,const std::string& val)
			{
				lua_pushlstring(L,val.c_str(),val.length());
			}

			static std::string cast(lua_State* L,int index)
			{
				assert(lua_isstring(L,index) || lua_isnil(L,index));
				size_t len = 0;
				const char* s = lua_tolstring(L,index,&len);
				return std::string(s,len);
			}
		};

		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		struct cast
		{
			template<class T1,class T2> static T1* to(T2* p)
			{
				return static_cast<T1*>(p);
			}
		};

		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		template<class T1,class T2> ptrdiff_t offset()
		{
			T2* p2 = (T2*)1000000L;
			return (ptrdiff_t)cast::to<T1>(p2) - (ptrdiff_t)cast::to<T2>(p2);
		};


		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		struct parent
		{
			ptrdiff_t offset;
			lua_CFunction fn_cast;
			lua_CFunction fn_index;
			char* classname;
		};

		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		template<class T> struct object
		{
			struct proxy
			{
				T* obj;
				bool gc;
				ptrdiff_t offset;
				int meta;
			};

			// ***************************************************************************

			template<class FN> struct method_slow
			{
				struct method_proxy
				{
					FN fn;
				};

				template<class RT> struct helper
				{
					static int invoke(lua_State* L)
					{
						int l1 = lua_type(L,1);
						int l2 = lua_type(L,2);
						proxy* _proxy = (proxy*)lua_touserdata(L,2);
						T* obj = (T*)((char*)_proxy->obj + _proxy->offset);
						_proxy->offset = 0;

						method_proxy* mproxy = (method_proxy*)lua_touserdata(L,1);

						stack_helper<RT>::push(L,method<FN,99999>::helper<RT>::call(*obj,mproxy->fn,L,3) );
						return 1;					
					}
				};

				template<> struct helper<void>
				{
					static int invoke(lua_State* L)
					{
						proxy* _proxy = (proxy*)lua_touserdata(L,2);
						T* obj = (T*)((char*)_proxy->obj + _proxy->offset);
						_proxy->offset = 0;

						method_proxy* mproxy = (method_proxy*)lua_touserdata(L,1);

						method<FN,99999>::helper<void>::call(*obj,mproxy->fn,L,3)
					}
				};
			};

			// ***************************************************************************

			template<class FN,long N> struct method
			{
				static FN fn;

				// ***************************************************************************

				template<class RT> struct helper
				{
					static int invoke(lua_State* L)
					{
						proxy* _proxy = (proxy*)lua_touserdata(L,1);
						T* obj = (T*)((char*)_proxy->obj + _proxy->offset);
						_proxy->offset = 0;

						stack_helper<RT>::push(L,call(*obj,fn,L,2));
						return 1;
					}

					static RT call(T& caller,RT (T::*func)(),lua_State* L,int index)
					{
						return (caller.*func)();
					}

					template<class P1> static RT call(T& caller,RT (T::*func)(P1),lua_State* L,int index)
					{
						return (caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0)
							);
					}

					template<class P1,class P2> static RT call(T& caller,RT (T::*func)(P1,P2),lua_State* L,int index)
					{
						return (caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1)
							);
					}

					template<class P1,class P2,class P3> static RT call(T& caller,RT (T::*func)(P1,P2,P3),lua_State* L,int index)
					{
						return (caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1),
							stack_helper<P3>::cast(L,index + 2)
							);
					}

					template<class P1,class P2,class P3,class P4> static RT call(T& caller,RT (T::*func)(P1,P2,P3,P4),lua_State* L,int index)
					{
						return (caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1),
							stack_helper<P3>::cast(L,index + 2),
							stack_helper<P4>::cast(L,index + 3)
							);
					}

					template<class P1,class P2,class P3,class P4,class P5> static RT call(T& caller,RT (T::*func)(P1,P2,P3,P4,P5),lua_State* L,int index)
					{
						return (caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1),
							stack_helper<P3>::cast(L,index + 2),
							stack_helper<P4>::cast(L,index + 3),
							stack_helper<P5>::cast(L,index + 4)
							);
					}
				};

				// ***************************************************************************

				template<> struct helper<void>
				{
					static int invoke(lua_State* L)
					{
						proxy* _proxy = (proxy*)lua_touserdata(L,1);
						T* obj = (T*)((char*)_proxy->obj + _proxy->offset);
						_proxy->offset = 0;

						L,call(*obj,fn,L,2);
						return 0;
					}

					static void call(T& caller,void (T::*func)(),lua_State* L,int index)
					{
						(caller.*func)();
					}

					template<class P1> static void call(T& caller,void (T::*func)(P1),lua_State* L,int index)
					{
						(caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0)
							);
					}

					template<class P1,class P2> static void call(T& caller,void (T::*func)(P1,P2),lua_State* L,int index)
					{
						(caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1)
							);
					}

					template<class P1,class P2,class P3> static void call(T& caller,void (T::*func)(P1,P2,P3),lua_State* L,int index)
					{
						(caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1),
							stack_helper<P3>::cast(L,index + 2)
							);
					}

					template<class P1,class P2,class P3,class P4> static void call(T& caller,void (T::*func)(P1,P2,P3,P4),lua_State* L,int index)
					{
						(caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1),
							stack_helper<P3>::cast(L,index + 2),
							stack_helper<P4>::cast(L,index + 3)
							);
					}

					template<class P1,class P2,class P3,class P4,class P5> static void call(T& caller,void (T::*func)(P1,P2,P3,P4,P5),lua_State* L,int index)
					{
						(caller.*func)
							(
							stack_helper<P1>::cast(L,index + 0),
							stack_helper<P2>::cast(L,index + 1),
							stack_helper<P3>::cast(L,index + 2),
							stack_helper<P4>::cast(L,index + 3),
							stack_helper<P5>::cast(L,index + 4)
							);
					}
				};
			};
		};

		template<class T> template<class FN,long N> FN object<T>::method<FN,N>::fn = NULL;

		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		template<class FN> struct method_slow
		{
			struct method_proxy
			{
				FN fn;
			};

			template<class RT> struct helper
			{
				static int invoke(lua_State* L)
				{
					method_proxy* mproxy = (method_proxy*)lua_touserdata(L,1);

					stack_helper<RT>::push(L,method<FN,99999>::helper<RT>::call(mproxy->fn,L,2) );
					return 1;					
				}
			};

			template<> struct helper<void>
			{
				static int invoke(lua_State* L)
				{
					method_proxy* mproxy = (method_proxy*)lua_touserdata(L,1);

					method<FN,99999>::helper<void>::call(mproxy->fn,L,2)
				}
			};
		};

		template<class FN,long N> struct method
		{
			static FN fn;

			template<class RT> struct helper
			{
				static int invoke(lua_State* L)
				{
					stack_helper<RT>::push(L,call(fn,L,1));
					return 1;
				}


				static RT call(RT (*func)(),lua_State* L,int index)
				{
					return (*func)();

				}

				template<class P1> static RT call(RT (*func)(P1),lua_State* L,int index)
				{
					return (*func)(
									stack_helper<P1>::cast(L,index + 0)
								);
					
				}

				template<class P1,class P2> static RT call(RT (*func)(P1,P2),lua_State* L,int index)
				{
					return (*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						);

				}

				template<class P1,class P2,class P3> static RT call(RT (*func)(P1,P2,P3),lua_State* L,int index)
				{
					return (*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						stack_helper<P3>::cast(L,index + 2)
						);

				}

				template<class P1,class P2,class P3,class P4> static RT call(RT (*func)(P1,P2,P3,P4),lua_State* L,int index)
				{
					return (*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						stack_helper<P3>::cast(L,index + 2),
						stack_helper<P4>::cast(L,index + 3)
						);

				}

				template<class P1,class P2,class P3,class P4,class P5> static RT call(RT (*func)(P1,P2,P3,P4,P5),lua_State* L,int index)
				{
					return (*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						stack_helper<P3>::cast(L,index + 2),
						stack_helper<P4>::cast(L,index + 3),
						stack_helper<P5>::cast(L,index + 4)
						);

				}
			};

			template<> struct helper<void>
			{
				static int invoke(lua_State* L)
				{
					L,call(fn,L,1);
					return 0;
				}


				static void call(void (*func)(),lua_State* L,int index)
				{
					(*func)();
				}

				template<class P1> static void call(void (*func)(P1),lua_State* L,int index)
				{
					(*func)(
						stack_helper<P1>::cast(L,index + 0)
						);

				}

				template<class P1,class P2> static void call(void (*func)(P1,P2),lua_State* L,int index)
				{
					(*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						);

				}

				template<class P1,class P2,class P3> static void call(void (*func)(P1,P2,P3),lua_State* L,int index)
				{
					(*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						stack_helper<P3>::cast(L,index + 2)
						);

				}

				template<class P1,class P2,class P3,class P4> static void call(void (*func)(P1,P2,P3,P4),lua_State* L,int index)
				{
					(*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						stack_helper<P3>::cast(L,index + 2),
						stack_helper<P4>::cast(L,index + 3)
						);

				}

				template<class P1,class P2,class P3,class P4,class P5> static void call(void (*func)(P1,P2,P3,P4,P5),lua_State* L,int index)
				{
					(*func)(
						stack_helper<P1>::cast(L,index + 0),
						stack_helper<P2>::cast(L,index + 1),
						stack_helper<P3>::cast(L,index + 2),
						stack_helper<P4>::cast(L,index + 3),
						stack_helper<P5>::cast(L,index + 4)
						);

				}
			};
		};

		template<class FN,long N> FN method<FN,N>::fn = NULL;

		// ***************************************************************************
		// ***************************************************************************
		// ***************************************************************************

		template<class T> struct constructor_helper0
		{
			static int create(lua_State* L)
			{
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_newuserdata(L,sizeof(object<T>::proxy)));
				_proxy->gc = true;
				_proxy->offset = 0;
				_proxy->obj = new T();
				_proxy->meta = class_name<T>::meta;
				lua_getref(L,class_name<T>::meta);
				lua_setmetatable(L,-2);
				return 1;
			}
		};

		template<class T,class P1> struct constructor_helper1
		{
			static int create(lua_State* L)
			{
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_newuserdata(L,sizeof(object<T>::proxy)));
				_proxy->gc = true;
				_proxy->offset = 0;
				_proxy->meta = class_name<T>::meta;
				
				_proxy->obj = new T
					(
						stack_helper<P1>::cast(L,1)
					);
				lua_getref(L,class_name<T>::meta);
				lua_setmetatable(L,-2);
				return 1;
			}
		};

		template<class T,class P1,class P2> struct constructor_helper2
		{
			static int create(lua_State* L,P1 p1,P2 p2)
			{
				object<T>::proxy* _proxy = (object<T>::proxy*)(lua_newuserdata(L,sizeof(object<T>::proxy)));
				_proxy->gc = true;
				_proxy->offset = 0;
				_proxy->meta = class_name<T>::meta;
				_proxy->obj = new T
					(
						stack_helper<P1>::cast(L,1),
						stack_helper<P1>::cast(L,2)
					);
				lua_getref(L,class_name<T>::meta);
				lua_setmetatable(L,-2);
				return 1;
			}
		};
	}
	

	// ---------------------------------------------------------------------------------------------------------

	template<class T> class	class_helper
	{
		enum { MAX_PARENTS = 20, };
	public:
		class_helper(lua_State* _L,const char* _name)
		{
			L = _L;
			strcpy_s(class_name<T>::name,_name);

			lua_newtable(L);
			lua_setglobal(L,class_name<T>::name);

			lua_newtable(L);
			
			void* p = lua_newuserdata(L,sizeof(_detail::parent) * MAX_PARENTS);
			memset(p,0,sizeof(_detail::parent) * MAX_PARENTS);
			lua_setfield(L,-2,_base_classes);

			lua_pushcfunction(L,&class_helper<T>::cast);
			lua_setfield(L,-2,_cast);

			class_name<T>::meta = lua_ref(L,true);

			make_index();
			dtor();
		}

		template<class PARENT> class_helper<T>& inherit()
		{
			
			lua_getref(L,class_name<T>::meta);
			lua_getfield(L,-1,_base_classes);
			_detail::parent* parents = (_detail::parent*)lua_touserdata(L,-1);
			lua_pop(L,2);

			while(parents->fn_index != NULL)
				++parents;

			parents->fn_index = &class_helper<PARENT>::_index;
			parents->offset = _detail::offset<PARENT,T>();
			parents->classname = &class_name<PARENT>::name[0];
			parents->fn_cast = &class_helper<PARENT>::cast;
			return *this;
		}

		class_helper<T>& make_index()
		{
			lua_getref(L,class_name<T>::meta);			
			lua_pushcfunction(L,&_index);
			lua_setfield(L,-2,"__index");
			lua_pop(L,1);

			return *this;
		}
	private:
		class_helper<T>& dtor()
		{
			lua_getref(L,class_name<T>::meta);			
			lua_pushcfunction(L,&_gc);
			lua_setfield(L,-2,"__gc");
			lua_pop(L,1);

			return *this;
		}
	public:
		class_helper<T>& ctor()
		{
			lua_getglobal(L,class_name<T>::name);			
			lua_pushcfunction(L,&_detail::constructor_helper0<T>::create);
			lua_setfield(L,-2,"new");
			lua_pop(L,1);
			return *this;
		}

		template<class P1> class_helper<T>& ctor()
		{
			lua_getglobal(L,class_name<T>::name);			
			lua_CFunction lfn = &_detail::constructor_helper1<T,P1>::create;
			lua_pushcfunction(L,lfn);
			lua_setfield(L,-2,"new");
			lua_pop(L,1);
			return *this;
		}

		template<class P1,class P2> class_helper<T>& ctor()
		{
			lua_getglobal(L,class_name<T>::name);
			lua_CFunction lfn = &_detail::constructor_helper2<T,P1,P2>::create
			lua_pushcfunction(L,lfn);
			lua_setfield(L,-2,"new");
			lua_pop(L,1);
			return *this;
		}

		template<class RT,long N,class FN> class_helper<T>& method_fast(const char* name,FN fn)
		{
			assert((_detail::object<T>::method<FN,N>::fn) == NULL);

			_detail::object<T>::method<FN,N>::fn = fn;

			lua_CFunction lfn = &_detail::object<T>::method<FN,N>::helper<RT>::invoke;
			lua_getref(L,class_name<T>::meta);
			lua_pushcfunction(L,lfn);
			lua_setfield(L,-2,name);
			lua_pop(L,1);
			return *this;
		}

		template<class RT,class FN> class_helper<T>& method_slow(const char* name,FN fn)
		{
			lua_CFunction lfn = _detail::object<T>::method_slow<FN>::helper<RT>::invoke;

			lua_getref(L,class_name<T>::meta);

			_detail::object<T>::method_slow<FN>::method_proxy* _proxy = (_detail::object<T>::method_slow<FN>::method_proxy*)lua_newuserdata(L,sizeof(_detail::object<T>::method_slow<FN>::method_proxy));
			_proxy->fn = fn;
			lua_newtable(L);
			lua_pushcfunction(L,lfn);
			lua_setfield(L,-2,"__call");
			lua_setmetatable(L,-2);

			lua_setfield(L,-2,name);
			lua_pop(L,1);
			return *this;
		}
	public:
		static int _index(lua_State* L)
		{
			_detail::object<T>::proxy* _proxy = (_detail::object<T>::proxy*)lua_touserdata(L,1);
			const char* name = lua_tostring(L,2);

			if(strcmp(name,_class_name) == 0)
			{
				lua_pushstring(L,class_name<T>::name);
				return 1;
			}

			lua_getref(L,class_name<T>::meta);
			lua_pushvalue(L,2);
			lua_gettable(L,-2);

			if(lua_type(L,-1) != LUA_TNIL)
			{
				lua_remove(L,-2);
				return 1;
			}

			lua_pop(L,1); // nil

			lua_getfield(L,-1,_base_classes);
			_detail::parent* parents = (_detail::parent*)lua_touserdata(L,-1);
			lua_pop(L,2);

			while(parents->fn_index != NULL)
			{
				if(parents->fn_index(L) == 1)
				{
					_proxy->offset += parents->offset;
					return 1;
				}
				++parents;
			}

			return 0;
		}

		static int _gc(lua_State* L)
		{
			_detail::object<T>::proxy* _proxy = (_detail::object<T>::proxy*)lua_touserdata(L,1);
			if(_proxy->gc)
				delete _proxy->obj;
			_proxy->obj = NULL;
			_proxy->gc = false;
			return 0;
		}

		static int cast(lua_State* L)
		{
			void* i = lua_touserdata(L,1);
			const char* classname = (const char*)lua_tointeger(L,2);

			if(classname == class_name<T>::name)
			{
				lua_pushlightuserdata(L,i);
				return 1;
			}

			lua_getref(L,class_name<T>::meta);
			lua_getfield(L,-1,_base_classes);
			_detail::parent* parents = (_detail::parent*)lua_touserdata(L,-1);
			lua_pop(L,2);

			while(parents->fn_cast != NULL)
			{
				lua_pushcfunction(L,parents->fn_cast);
				lua_pushvalue(L,1);
				lua_pushvalue(L,2);
				lua_pcall(L,2,1,0);
				
				if(lua_type(L,-1) == LUA_TLIGHTUSERDATA)
				{
					void* o = lua_touserdata(L,-1);
					lua_pushlightuserdata(L,(char*)o + parents->offset);
					lua_remove(L,-2);
					return 1;
				}
				lua_pop(L,1);//nil
				++parents;
			}
			return 0;
		}
	private:
		lua_State* L;
	};

	// ---------------------------------------------------------------------------------------------------------


	template<class T> class_helper<T> class_(lua_State* L,const char* name)
	{
		return class_helper<T>(L,name);
	}

	template<class RT,long N,class FN> void method_fast(lua_State* L,const char* name,FN fn)
	{
		assert((_detail::method<FN,N>::fn) == NULL);
		_detail::method<FN,N>::fn = fn;
		lua_CFunction lfn = &_detail::method<FN,N>::helper<RT>::invoke;
		lua_pushcfunction(L,lfn);
		lua_setglobal(L,name);
	}

	template<class RT,class FN>  void method_slow(lua_State* L,const char* name,FN fn)
	{
		lua_CFunction lfn = _detail::method_slow<FN>::helper<RT>::invoke;

		_detail::method_slow<FN>::method_proxy* _proxy = (_detail::method_slow<FN>::method_proxy*)lua_newuserdata(L,sizeof(_detail::method_slow<FN>::method_proxy));
		_proxy->fn = fn;
		lua_newtable(L);
		lua_pushcfunction(L,lfn);
		lua_setfield(L,-2,"__call");
		lua_setmetatable(L,-2);

		lua_setglobal(L,name);
	}

	// ---------------------------------------------------------------------------------------------------------

	class reference
	{
	public:
		reference(lua_State* _L,const char* name)
			: L(_L)
		{
			lua_getglobal(L,name);
			ref = lua_ref(L,true);
		}

		reference(lua_State* _L)
			: L(_L)
		{
			ref = lua_ref(L,true);
		}

		~reference()
		{
			lua_unref(L,ref);
		}

		void push()
		{
			lua_getref(L,ref);
		}

		template<class T> T cast()
		{
			temporary t(L,*this);
			return t.cast<T>();
		}
	private:
		reference(const reference&);
		reference operator=(const reference&);
	private:
		lua_State* L;
		int ref;
	};

	// ---------------------------------------------------------------------------------------------------------

	class temporary
	{
	public:
		temporary(lua_State* _L,reference& r)
		{
			L = _L;
			r.push();
			_stack_index = -1;
		}
		temporary(lua_State* _L,int stack_index)
		{
			L = _L;
			_stack_index = stack_index;
		}

		temporary(lua_State* _L,const char* name,int table = LUA_GLOBALSINDEX)
		{
			L = _L;
			
			if(!lua_istable(L,table) && !lua_isuserdata(L,table))
				lua_pushnil(L);
			else
				lua_getfield(L,table,name);
			_stack_index = -1;
		}

		temporary(lua_State* _L,const char* name,reference& r)
		{
			L = _L;

			r.push();

			if(!lua_istable(L,-1) && !lua_isuserdata(L,-1))
				lua_pushnil(L);
			else
				lua_getfield(L,-1,name);
			
			lua_remove(L,-2);

			_stack_index = -1;
		}

		~temporary()
		{
			if(_stack_index < 0)
				lua_pop(L,1);
		}
	public:
		inline int type() 
		{
			return lua_type(L,_stack_index);
		}
	public:
		template<class T> T cast()
		{
			return _detail::stack_helper<T>::cast(L,_stack_index);
		}

		template<class RT> RT invoke()
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			lua_pcall(L,0,1,0);
			RT rt = _detail::stack_helper<RT>::cast(L,-1);
			lua_pop(L,1);
			return rt;
		}

		template<class RT,class P1> RT invoke(P1 p1)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			lua_pcall(L,1,1,0);
			RT rt = _detail::stack_helper<RT>::cast(L,-1);
			lua_pop(L,1);
			return rt;
		}

		template<class RT,class P1,class P2> RT invoke(P1 p1,P2 p2)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			lua_pcall(L,2,1,0);
			RT rt = _detail::stack_helper<RT>::cast(L,-1);
			lua_pop(L,1);
			return rt;
		}

		template<class RT,class P1,class P2,class P3> RT invoke(P1 p1,P2 p2,P3 p3)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			_detail::stack_helper<P3>::push(L,p3);
			lua_pcall(L,3,1,0);
			RT rt = _detail::stack_helper<RT>::cast(L,-1);
			lua_pop(L,1);
			return rt;
		}

		template<class RT,class P1,class P2,class P3,class P4> RT invoke(P1 p1,P2 p2,P3 p3,P4 p4)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			_detail::stack_helper<P3>::push(L,p3);
			_detail::stack_helper<P4>::push(L,p4);
			lua_pcall(L,4,1,0);
			RT rt = _detail::stack_helper<RT>::cast(L,-1);
			lua_pop(L,1);
			return rt;
		}

		template<class RT,class P1,class P2,class P3,class P4,class P5> RT invoke(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			_detail::stack_helper<P3>::push(L,p3);
			_detail::stack_helper<P4>::push(L,p4);
			_detail::stack_helper<P5>::push(L,p5);
			lua_pcall(L,5,1,0);
			RT rt = _detail::stack_helper<RT>::cast(L,-1);
			lua_pop(L,1);
			return rt;
		}
	public:
		void invoke_nr()
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			lua_pcall(L,0,0,0);
		}


		template<class P1> void invoke_nr(P1 p1)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			lua_pcall(L,1,0,0);
		}

		template<class P1,class P2> void invoke_nr(P1 p1,P2 p2)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			lua_pcall(L,2,0,0);
		}

		template<class P1,class P2,class P3> void invoke_nr(P1 p1,P2 p2,P3 p3)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			_detail::stack_helper<P3>::push(L,p3);
			lua_pcall(L,3,0,0);
		}

		template<class P1,class P2,class P3,class P4> void invoke_nr(P1 p1,P2 p2,P3 p3,P4 p4)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			_detail::stack_helper<P3>::push(L,p3);
			_detail::stack_helper<P4>::push(L,p4);
			lua_pcall(L,4,0,0);
		}

		template<class P1,class P2,class P3,class P4,class P5> void invoke_nr(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
		{
			assert(type() == LUA_TFUNCTION);
			lua_pushvalue(L,_stack_index);
			_detail::stack_helper<P1>::push(L,p1);
			_detail::stack_helper<P2>::push(L,p2);
			_detail::stack_helper<P3>::push(L,p3);
			_detail::stack_helper<P4>::push(L,p4);
			_detail::stack_helper<P5>::push(L,p5);
			lua_pcall(L,5,0,0);
		}
	private:
		int _stack_index;
		lua_State* L;
	};
}

#endif // _pw_lua_