



r = TestB.new()
v = Test2.new(r)
v.pp = r
v.pp.n = 43252
print("v.pp.n=" .. v.pp.n)
v.pp = nil


print("r.n=" .. r.n)
r.n  = 495934

print("v.n = " .. v.n)

print( "result=" .. tostring(v:printn()))

v:print3(8787)
v:print2(5943)


global_print()

print( "result=" .. tostring(v:printn()))

print( "name=" .. v.classname)


print( "global_print=" .. global_print())

print("classname=" .. tostring(v.classname))
print("classname=" .. tostring(r.classname))


