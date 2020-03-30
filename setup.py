import os
import sys
import subprocess
import pybind11


from setuptools import setup, Extension

__version__ = '0.2.3'
SRC = "./src/data.hpp"
module_name = "fastMapper"
ext_module_name = "fastMapper_pybind"

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
            # Path to fasttext source code
            SRC,
        ],
        language='c++',
        extra_compile_args=["-std=c++11"],
    ),
]

setup(name=module_name,
      version='0.0.1',
      description='A print test for PyPI',
      author='f20500909',
      author_email='me@lightgoing.com',
      url='https://www.python.org/',
      license='mei xiang hao ',
      keywords='map ',
      project_urls={
          # 'Documentation': 'https://packaging.python.org/tutorials/distributing-packages/',
          # 'Funding': 'https://donate.pypi.org',
          'Source': 'https://github.com/f20500909/fastMapper',
          # 'Tracker': 'https://github.com/pypa/sampleproject/issues',
      },
      packages=[module_name],
      # package_dir表示一种映射关系,此处表示包的根目录为当前的python文件夹
      package_dir={'': 'python'},
      install_requires=[],
      python_requires='>=3',
      ext_modules=ext_modules,
      )
