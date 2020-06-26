## Introduce
  - a random map generation tool,  can generate new random map by using the existing map
 
 ![result](https://img-blog.csdnimg.cn/2020062615170475.png)

## How to use
   first download project and unzip
   cd ./fastMapper
   
 - with python


     python3 setup.py install
     python3 ./script demo.py
      - example
    from fastMapper import fastMapper
    module = fastMapper()
    module.out_height = 50
    module.out_width = 60
    module.symmetry = 1
    module.N = 2
    module.channels = 3
    module.log = 1
    module.input_data = "../samples/o3.png"
    module.output_data = "../output/done.png"
    module.type = "img"
    module.run() 

 with cpp
 
     cd script
     ./demo.sh
 
    
 with lua
      
     ./script/to_lua.lua
   


## How it work ?

 ![pic](https://img-blog.csdnimg.cn/20200514150849493.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4ODcxMTcz,size_16,color_FFFFFF,t_70)

## my blog about this project
- https://blog.csdn.net/qq_38871173/category_10003699.html

## reference
- https://github.com/mxgmn/aveunctionollapse


