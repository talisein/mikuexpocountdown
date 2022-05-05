project('mikuexpocountdown', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.1.0',
        default_options : ['c_std=c17',
                           'cpp_std=c++2a',
                           'buildtype=release'])

cpp_compiler = meson.get_compiler('cpp')
gtkmm_dep  = dependency('gtkmm-3.0',   version : '>=3.24.0')

is_not_windows = host_machine.system() != 'windows'

date_dep = dependency('date',
                      fallback: ['hinnant-date', 'tz_dep'],
                      default_options: ['use_system_tzdb=@0@'.format(is_not_windows),
                                        'default_library=static'])

subdir('res')
subdir('src')
