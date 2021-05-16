project(
  'portable-endianness',
  ['c', 'cpp'],
  version : '1.0',
  default_options : [
    'buildtype=release',
    'warning_level=3',
    'c_std=c99',
    'cpp_std=c++11',
    'b_ndebug=if-release'],
  license : 'Unlicense')

pe_include_dir = include_directories('include')

pe_lib = library(
  'portable-endianness',
  files(['src/portable-endianness.c']),
  soversion : 0,
  version : meson.project_version(),
  install : not meson.is_subproject(),
  include_directories : pe_include_dir)

pe_test = executable(
  'pe-test',
  files(['tests/test-portable-endianness.cpp']),
  include_directories : pe_include_dir,
  link_with : pe_lib,
  install : false)

test(
  'portable-endianness test',
  pe_test)
