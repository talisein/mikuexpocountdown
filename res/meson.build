gnome = import('gnome')

res = gnome.compile_resources('mikuexpocountdown-resources',
                              'mikuexpocountdown.gresource.xml',
                              install_header : false,
                              export : false,
                              c_name : 'mikuexpocountdown_resources')



# --- Desktop File ---
desktop_file = files('dance._39music.MikuExpoCountdown.desktop')

# If found, use a tool to validate the desktop file.
desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

install_data(desktop_file,
             install_dir: get_option('datadir') / 'applications')

# --- Icon ---
install_data('dance._39music.MikuExpoCountdown.svg',
             install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps')

# --- Appstream ---
metainfo_file = files('dance._39music.MikuExpoCountdown.metainfo.xml')

ascli_exe = find_program('appstreamcli', required: false)
if ascli_exe.found()
  test('validate metainfo file',
        ascli_exe,
        args: ['validate',
               '--no-net',
               '--pedantic',
               metainfo_file]
  )
endif

install_data(metainfo_file,
             install_dir: get_option('datadir') / 'metainfo')
