project('mikuexpocountdown', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.5.0',
        default_options : ['c_std=c17',
                           'cpp_std=c++2a',
                           'buildtype=release'])

gtk_dep    = dependency('gtk+-3.0')
cpp_compiler = meson.get_compiler('cpp')
glibmm_dep = dependency('glibmm-2.4',  version : '>=2.44.0')
giomm_dep  = dependency('giomm-2.4',  version : '>=2.44.0')
gtkmm_dep  = dependency('gtkmm-3.0',   version : '>=3.4.0')

subdir('res')
subdir('src')
