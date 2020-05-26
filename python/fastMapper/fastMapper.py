import fastMapper_pybind as fp_pybind


class fastMapper:
    # -h 40 -w 40 -s 8  -N 2 -i ../samples/City.png -o ../res/done.jpg  -t svg

    def __init__(self, high=40, width=40, s=8, N=2, input="null", output="null", type="null"):
        self.high = high
        self.width = width
        self.s = s
        self.N = N
        self.input = input
        self.output = output
        self.type = type
        print("init....")

    def test(self):
        print(fp_pybind.foo())

    def run(self):
        fp_pybind.run(self.high, self.width, self.s, self.N, self.input, self.output, self.type)


if __name__ == "__main__":
    module = fastMapper()
    module.test()
    fp_pybind.run(40, 40, 8, 2, "../../samples/City.png", "../../res/done.png", "svg")
