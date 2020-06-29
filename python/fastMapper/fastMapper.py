import fastMapper_pybind as fp_pybind

class fastMapper:
    # single_run(out_height, out_width, symmetry, N, channels, log, input_data, output_data, type);

    def __init__(self,
                 out_height=40,
                 out_width=40,
                 symmetry=8,
                 N=2,
                 channels=3,
                 log=1,
                 input_data="null",
                 output_data="null",
                 type="null"):
        self.out_height = out_height
        self.out_width = out_width
        self.symmetry = symmetry
        self.N = N
        self.channels = channels
        self.log = log
        self.input_data = input_data
        self.output_data = output_data
        self.type = type
        print("init succes ....")

    # single_run(out_height, out_width, symmetry, N, channels, log, input_data, output_data, type);

    def run(self):
        fp_pybind.run(self.out_height, self.out_width, self.symmetry, self.N, self.channels, self.log,self.input_data, self.output_data, self.type)


if __name__ == "__main__":
    print()
    # module = fastMapper()
    # module.test()
    # fp_pybind.run(40, 40, 8, 2, "../../samples/City.png", "../../res/done.png", "svg")
