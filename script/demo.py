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
