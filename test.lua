



r = TestB.new()
v = Test2.new(r)

v:print3()
v:print2(5943)

global_print()

print( "result=" .. tostring(v:printn()))
print( "name=" .. v.name)

print( "global_print=" .. global_print())

print("classname=" .. tostring(v.name))
print("classname=" .. tostring(r.name))