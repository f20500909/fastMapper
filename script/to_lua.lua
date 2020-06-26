package.cpath = package.cpath .. ';E:/CPP/WFC/script/lib/?.dll;'
print(package.cpath)


require "fastmapper_dll" --引入包

--在调用时，必须是package.function


-- -h 20 -w 40 -s 8  -N 2 -i ../samples/o.png -o ../res/done.jpg  -t svg

print(fastmapper_dll.run(20, 40, 8, 2,1,1, "E:/CPP/WFC/samples/City.png", "./done.jpg", "svg"));
--Mydll.run(20, 40, 8, 2)
