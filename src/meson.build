src = ['main.cxx', 'searchprovider_stub.cpp', 'events.cpp', 'search_provider.cpp']

exe = executable('mikuexpo-countdown', src, res,
                 dependencies : [gtkmm_dep, date_dep],
                 install : true,
                 gui_app : true,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                )

printer_exe = executable('mikuexpo-printer', 'printer.cxx',
                 dependencies : [date_dep],
                 install : false,
                 gui_app : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )

printer_exe = executable('mikuexpo-tweets', 'tweets.cxx',
                 dependencies : [date_dep],
                 install : false,
                 gui_app : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )
