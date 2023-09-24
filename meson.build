project('mikuexpocountdown', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '1.1.0',
        default_options : ['c_std=c17',
                           'cpp_std=c++20',
                           'buildtype=release'])

cpp_compiler = meson.get_compiler('cpp')

cpp_compiler.compute_int('__cpp_lib_chrono >= 201907', prefix: '#include <version>')

gtkmm_dep  = dependency('gtkmm-4.0',   version : '>=4.8.0')
adw_dep = dependency('libadwaita-1', version: '>=1.3.0')

is_not_windows = host_machine.system() != 'windows'

date_dep = dependency('date',
                      fallback: ['hinnant-date', 'tz_dep'],
                      default_options: ['use_system_tzdb=@0@'.format(is_not_windows),
                                        'default_library=static'])

subdir('res')
subdir('src')
