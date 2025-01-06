gnome = import('gnome')

res = gnome.compile_resources('mikuexpocountdown-resources',
                              'mikuexpocountdown.gresource.xml',
                              install_header : false,
                              export : false,
                              c_name : 'mikuexpocountdown_resources')

# GSettings

schemas_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0' / 'schemas'
settings_schemas = [ 'dance._39music.MikuExpoCountdown.gschema.xml' ]
install_data(settings_schemas, install_dir: schemas_dir)
gnome.compile_schemas(depend_files: files(settings_schemas))
gnome.post_install(glib_compile_schemas: true)
test('validate-schema',
  find_program('glib-compile-schemas'),
  args: ['--strict', '--dry-run', meson.current_source_dir()],
)
# --- Desktop File ---
desktop_file = files('dance._39music.MikuExpoCountdown.desktop')

# If found, use a tool to validate the desktop file.
desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file])
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

# --- dbus service file ---
dbus_conf = configuration_data()
dbus_conf.set('BIN_DIR', get_option('prefix') / get_option('bindir'))
dbus_service = configure_file(input: 'dance._39music.MikuExpoCountdown.service.in',
                              output: 'dance._39music.MikuExpoCountdown.service',
                              configuration: dbus_conf,
                              install: true,
                              install_dir: get_option('datadir') / 'dbus-1' / 'services')

# -- systemd service ---
install_data('dance._39music.MikuExpoCountdown.SearchProvider.service.systemd',
             install_dir: get_option('prefix') / 'lib' / 'systemd' / 'user',
             rename: 'dance._39music.MikuExpoCountdown.SearchProvider.service')

# gnome-shell search provider ini
install_data('search-provider.ini',
             install_dir: get_option('datadir') / 'gnome-shell' / 'search-providers',
             rename: 'dance._39music.MikuExpoCountdown.search-provider.ini')

# IANA tzdata file until gcc bug is fixed in flatpak gcc
bundled_iana_dir = get_option('datadir') / 'mikuexpo-countdown'
if get_option('bundle_iana_tzdata')
  install_data('tzdata-2024a.zi',
               install_dir: bundled_iana_dir,
               rename: 'tzdata.zi')
endif
