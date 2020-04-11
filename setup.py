import os
import sys
import subprocess
import pybind11

from setuptools import setup, Extension

version = '0.0.1'
SRC = ["src","src/include"]
module_name = "fastMapper"
ext_module_name = "fastMapper_pybind"

src_cpp = ['src\\fastMapper_pybind.cpp', ]


def getSrc(SRC):
    res = []
    for k, _src in enumerate(SRC):
        src_files = map(str, os.listdir(_src))
        src_cpp = list(filter(lambda x: x.endswith('.hpp'), src_files))

        src_cpp = list(
            map(lambda x: str(os.path.join(_src, x)), src_cpp)
        )
        res.extend(src_cpp)
    return res


src_cpp = getSrc(SRC)
src_cpp.append('src/glove/py_bind/glove_pybind.cpp')


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        try:
            import pybind11
        except ImportError:
            if subprocess.call([sys.executable, '-m', 'pip', 'install', 'pybind11']):
                raise RuntimeError('pybind11 install failed.')

        self.user = user

    def __str__(self):
        return pybind11.get_include(self.user)


ext_modules = [
    Extension(
        ext_module_name,
        # Path to pybind11 headers
        src_cpp,
        include_dirs=[
            get_pybind_include(),
            get_pybind_include(user=True),
        ],
        language='c++',
        extra_compile_args=["-std=c++11 -D_hypot=hypot"],
    ),
]

setup(name=module_name,
      version=version,
      description=' fastMapper',
      author='f20500909',
      author_email='me@lightgoing.com',
      # url='https://www.python.org/',
      license='mei xiang hao ',
      keywords='map ',
      project_urls={
          'Documentation': 'https://github.com/f20500909/fastMapper',
          'Source': 'https://github.com/f20500909/fastMapper',
      },
      packages=[module_name],
      # package_dir表示一种映射关系,此处表示包的根目录为当前的python文件夹
      package_dir={'': 'python'},
      install_requires=[],
      python_requires='>=3',
      ext_modules=ext_modules,
      )
