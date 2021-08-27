

src = ['main.cxx']

date_deps = [date_proj.get_variable('date_dep'), date_proj.get_variable('tz_dep')]

exe = executable('mikuexpo_countdown', src, res,
                 dependencies : [gtkmm_dep, date_deps],
                 install : true,
                 gui_app : true,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                )

printer_exe = executable('mikuexpo_printer', 'printer.cxx',
                 dependencies : [date_deps],
                 install : false,
                 gui_app : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )
