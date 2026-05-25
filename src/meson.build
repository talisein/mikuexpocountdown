src = ['main.cxx', 'application.cpp', 'events.cpp', 'search_provider.cpp', 'grid.cpp', 'window.cpp']

conf_data = configuration_data()
conf_data.set10('is_tzdata_bundled', get_option('bundle_iana_tzdata'))
conf_data.set_quoted('tzdata_bundle_dir', get_option('prefix') / bundled_iana_dir)

conf = configure_file(input: 'config.h.in',
                      output: 'config.h',
                      configuration: conf_data)

exe = executable('mikuexpo-countdown', src, res, conf,
                 dependencies : [adw_dep, peel_dep, peel_gtk_dep],
                 install : true,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                )

printer_exe = executable('mikuexpo-printer', 'printer.cxx',
                 install : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )

printer_exe = executable('mikuexpo-tweets', 'tweets.cxx',
                 install : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )
