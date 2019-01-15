from setuptools import setup, Extension

bktree_module = Extension(
    'bktreelib',
    sources=["bktree/python_module wstring.cpp"],
    depends=["bktree/bktree.h","bktree/levenshtein_wstring.h"],
    extra_compile_args=["-std=c++11","-O0", "-g"]
)

setup(
    name="bktreelib",
    description="bktreelib C module",
    ext_modules=[bktree_module]
)




