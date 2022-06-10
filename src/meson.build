gnome = import('gnome')
sp_sources = gnome.gdbus_codegen(
  'shell-search-provider-generated',
  sources: '/usr/share/dbus-1/interfaces/org.gnome.ShellSearchProvider2.xml',
  interface_prefix : 'dance.39music.',
  namespace : 'MikuExpoCountdown',
)

src = ['main.cxx', 'searchprovider_stub.cpp']


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
