import os, threading
from waflib import TaskGen, Task, Errors, Context, Utils
from waflib.extras import buildcopy

out = 'build'
APPNAME = 'ogre-meshoptim'
ENABLE_DEBUG_SYMBOLS = '-g'
OPTIMIZATION = '-O3'

def options(opt):
  opt.load('compiler_cxx')
  opt.add_option('--release-build', action='store_true', default=False, help='If set, build with optimizations')

def configure(cnf):
  cnf.load('compiler_cxx')
  cnf.check_cfg(package='tinyxml2', uselib_store='tinyxml2', args=['--cflags', '--libs'])
  cnf.check_cfg(package='fmt', uselib_store='fmt', args=['--cflags', '--libs'])

  if cnf.options.release_build:
    cnf.env.append_value('CXXFLAGS', OPTIMIZATION)
  else:
    cnf.env.append_value('CXXFLAGS', ENABLE_DEBUG_SYMBOLS)

  # cnf.env.append_value('INCLUDES', '/Users/simonwarg/Development/llvm-project/libcxx/include')
  cnf.recurse('src/lexer')

def build(bld):
  bld.recurse('src/lexer')

  bld.stlib(
    target = 'docopt',
    features = 'cxx',
    includes = ['src/docopt'],
    export_includes = ['src/docopt'],
    uselib = '',
    use = '',
    cxxflags = ['-std=c++17', '-stdlib=libc++'],
    source = bld.path.ant_glob(incl = [
      'src/docopt/*.cpp',
    ])
  )

  bld.stlib(
    target = 'parser',
    features = 'cxx',
    includes = ['src', 'src/lexer/src'],
    export_includes = ['src', 'src/lexer/src'],
    uselib = '',
    use = '',
    cxxflags = ['-std=c++17', '-stdlib=libc++'],
    source = bld.path.ant_glob(incl = [
      'src/parser/*.cpp',
    ])
  )

  bld.stlib(
    target = 'ogre-meshoptim',
    features = 'cxx',
    includes = ['src'],
    export_includes = ['src'],
    uselib = 'tinyxml2 fmt',
    use = 'lexer',
    cxxflags = ['-std=c++17', '-stdlib=libc++'],
    source = bld.path.ant_glob(incl = [
      'src/meshoptim/*.cpp',
      'src/meshoptim/*.tests.cpp',
    ])
  )


  bld.program(
    target = 'tests',
    features = 'cxx',
    includes = ['src'],
    uselib = 'tinyxml2 fmt',
    use = 'lexer',
    cxxflags = ['-std=c++17', '-stdlib=libc++'],
    source = bld.path.ant_glob(incl = [
      'src/tests/*.cpp',
      'src/meshoptim/*.cpp',
      'src/meshoptim/*.tests.cpp',
      'src/parser/**/*.cpp'
    ])
  )

  bld.program(
    target = 'meshoptim',
    features = 'cxx',
    includes = ['src'],
    uselib = 'tinyxml2 fmt',
    use = 'lexer docopt parser ogre-meshoptim',
    cxxflags = ['-std=c++17', '-stdlib=libc++'],
    source = bld.path.ant_glob(incl = [
      'src/cli/*.cpp'
    ])
  )


  