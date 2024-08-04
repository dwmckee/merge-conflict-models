project('merge-conflict-models', 'cpp',
        default_options : ['cpp_std=c++20'],
        version: '0.1.0',
        license: 'MIT')

doctest_proj = subproject('doctest')
doctest_dep = doctest_proj.get_variable('doctest_dep')
# NB: Doctest dependency is needed on anything that includes doctest.h even if
# we use DOCTEST_CONFIG_DISABLE

library_files = [
  'basiccreator.cpp',
  'codeelement.cpp',
  'editcounter.cpp', 'editmutator.cpp',
  'interfaces.cpp',
  'projectfile.cpp', 'projectid.cpp', 'projectstate.cpp',
  'constrained_random.cpp']

library = static_library('libmerge-conflict-models', library_files,
                         cpp_args: '-DDOCTEST_CONFIG_DISABLE',
                         dependencies: doctest_dep)

executable('files_size_histogram', ['file_size_hist.cpp'],
           link_with: library,
          dependencies: doctest_dep)

executable('long_files', ['long_files_1.cpp'],
           link_with: library,
           dependencies: doctest_dep)

test_exe = executable('test-framework',
                      ['test-framework.cpp'] + library_files,
                      dependencies: doctest_dep)
test('unit tests', test_exe, args: [])

