from fastMapper import fastMapper


module = fastMapper(40, 40, 8, 2, 3, 1, "E:/CPP/WFC/samples/City.png", "done.jpg", "svg")
# -h 50 -w 60 -s 1 -N 2 -c 3 -l 1 -i ../samples/City.png -o ../res/done.jpg  -t img

res = module.run()

print(res)
