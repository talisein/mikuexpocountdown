

src = ['main.cxx', 'tz.cpp']

exe = executable('mikuexpo_countdown', src, res,
                 dependencies : [gtkmm_dep],
                 install : true,
                 gui_app : true,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true', '-DUSE_OS_TZDB', '-DAUTO_DOWNLOAD=0'],
                 )
