project('mikuexpocountdown', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.1.0',
        default_options : ['c_std=c17',
                           'cpp_std=c++2a',
                           'buildtype=release'])

cpp_compiler = meson.get_compiler('cpp')
gtkmm_dep  = dependency('gtkmm-3.0',   version : '>=3.24.0')

subdir('res')
subdir('src')
