from fastMapper import fastMapper

module = fastMapper()

module.test()

# -h 40 -w 40 -s 8  -N 2 -i ../samples/City.png -o ../res/done.jpg  -t svg
# module.run(40, 40, 8, 2, "../../samples/City.png", "../../res/done.jpg", "svg")
module.run()
