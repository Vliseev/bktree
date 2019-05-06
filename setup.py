from setuptools import setup, Extension

bktree_module = Extension(
    'bktree.bktreelib',
    sources=["src/python_module_wstring.cpp"],
    depends=["src/src.h","src/levenshtein_wstring.h"],
    extra_compile_args=["-std=c++11","-O3", "-g"]
)

setup(
    name="bktree",
    packages = ["bktree"],
    description="bktreelib C module",
    ext_modules=[bktree_module]
)




