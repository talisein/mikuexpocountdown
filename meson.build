project('mikuexpocountdown', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '2.2.0',
        default_options : ['c_std=c17',
                           'cpp_std=c++23',
                           'buildtype=release'])

cpp_compiler = meson.get_compiler('cpp')

cpp_compiler.compute_int('__cpp_lib_chrono >= 201907', prefix: '#include <version>')

adw_dep = dependency('libadwaita-1', version: '>=1.3.0')
peel_dep      = dependency('peel', fallback: ['peel', 'peel_dep'])
peel_gen_prog = find_program('peel-gen')
peel_gtk = custom_target('peel-codegen',
  command: [peel_gen_prog, '--recursive', '--out-dir', '@OUTDIR@',
            'Gtk', '4.0', 'Adw', '1', 'Gio', '2.0'],
  output: 'peel')
peel_gtk_dep = declare_dependency(sources: peel_gtk)

is_not_windows = host_machine.system() != 'windows'

subdir('res')
subdir('src')
