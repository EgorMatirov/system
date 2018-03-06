project('bacs_system', 'cpp',
        version : '0.1.0',
        license : 'apache2',
        meson_version : '>=0.45',
        default_options : [
            'cpp_std=c++17',
            'warning_level=3',
        ])

add_global_arguments('-DBOOST_ALL_DYN_LINK', language : 'cpp')

include = include_directories('include')

srcs = [
    'src/lib/builder.cpp',
    'src/lib/file.cpp',
    'src/lib/process.cpp',
    'src/lib/system_verifier.cpp',

    'src/lib/builders/compilable.cpp',
    'src/lib/builders/native_compilable.cpp',
    'src/lib/builders/gcc.cpp',
    'src/lib/builders/fpc.cpp',
    'src/lib/builders/interpretable.cpp',
    'src/lib/builders/python.cpp',
    'src/lib/builders/java.cpp',
    'src/lib/builders/mono.cpp',

    'src/lib/builders/project.cpp',
    'src/lib/builders/mono_develop.cpp',
]

deps = [
    dependency('threads'),
    dependency('bunsan_utility'),
    dependency('bacs_common'),
    dependency('bacs_problem'),
    dependency('yandex_contest_invoker'),
]


lib = shared_library('bacs_system', srcs,
    include_directories : include,
    dependencies : deps,
    install : true,
    version : meson.project_version(),
    soversion : meson.project_version(),
)

bacs_system_dep = declare_dependency(
    link_with : lib,
    dependencies : deps,
    include_directories : include,
)

pkg = import('pkgconfig')

pkg.generate(
    name : 'bacs_system',
    description : 'bacs::system helpers',
    libraries : [lib] + deps,
    version : meson.project_version(),
)

install_subdir('include',
               install_dir : get_option('includedir'),
               strip_directory : true)

subdir('tests')
