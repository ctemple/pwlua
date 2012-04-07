



r = TestB.new()
v = Test2.new(r)

v:print()
v:print2(5943)


print( "result=" .. tostring(v:printn()))
print( "name=" .. v.name)

print( "global_print=" .. global_print())

print("classname=" .. tostring(v.name))
print("classname=" .. tostring(r.name))