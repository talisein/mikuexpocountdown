src = ['main.cpp', 'application.cpp', 'events.cpp', 'search_provider.cpp', 'grid.cpp', 'window.cpp']

conf_data = configuration_data()
conf_data.set10('is_tzdata_bundled', get_option('bundle_iana_tzdata'))
conf_data.set_quoted('tzdata_bundle_dir', get_option('prefix') / bundled_iana_dir)

conf = configure_file(input: 'config.h.in',
                      output: 'config.h',
                      configuration: conf_data)

# A GUI program on Windows, so starting it opens no console window. With
# MSVC the windows subsystem expects WinMain(); the entry point keeps main().
exe_link_args = []
if not is_not_windows and meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
  exe_link_args += '/ENTRY:mainCRTStartup'
endif

exe = executable('mikuexpo-countdown', src, res, windows_res, conf,
                 dependencies : [adw_dep, peel_dep, peel_gtk_dep],
                 install : true,
                 win_subsystem : 'windows',
                 link_args : exe_link_args,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                )

test_variant_exe = executable('test-variant-utils', 'test_variant_utils.cpp',
                 dependencies: [adw_dep, peel_dep, peel_gtk_dep],
                 cpp_args: ['-D_GNU_SOURCE'])
test('variant-utils', test_variant_exe)

test_event_props_exe = executable('test-event-properties',
                 ['test_event_properties.cpp', 'events.cpp'], conf,
                 dependencies: [adw_dep, peel_dep, peel_gtk_dep],
                 cpp_args: ['-D_GNU_SOURCE'])
test('event-properties', test_event_props_exe)

# Both use <experimental/iterator>, which only libstdc++ has.
if is_not_windows
printer_exe = executable('mikuexpo-printer', 'printer.cpp',
                 install : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )

printer_exe = executable('mikuexpo-tweets', 'tweets.cpp',
                 install : false,
                 cpp_args : ['-D_GNU_SOURCE', '-DG_ENABLE_DEBUG=true']
                 )
endif
