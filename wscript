import os, threading
from waflib import TaskGen, Task, Errors, Context, Utils
from waflib.extras import buildcopy

out = 'build'
APPNAME = 'ogre-meshoptim'
ENABLE_DEBUG_SYMBOLS = '-g'

def options(opt):
  opt.load('compiler_cxx')

def configure(cnf):
  cnf.load('compiler_cxx')
  # cnf.env.append_value('INCLUDES', '/Users/simonwarg/Development/llvm-project/libcxx/include')

def build(bld):
  bld.program(
    target = 'tests',
    features = 'cxx',
    includes = ['src'],
    cxxflags = ['-std=c++17', '-stdlib=libc++', ENABLE_DEBUG_SYMBOLS],
    source = bld.path.ant_glob(incl = [
      'src/tests/*.cpp',
      'src/meshoptim/*.cpp',
      'src/meshoptim/*.tests.cpp'
    ])
  )


  