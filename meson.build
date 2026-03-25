project('aki', 'cpp',
  version : '0.1.0',
  default_options : ['warning_level=3', 'cpp_std=c++20']
)

# 依赖项
wlroots = dependency('wlroots', version : '>=0.17')
wayland_server = dependency('wayland-server')
wayland_protocols = dependency('wayland-protocols')
libglvnd = dependency('libglvnd')
libinput = dependency('libinput')
xkbcommon = dependency('xkbcommon')
pixman = dependency('pixman-1')
systemd = dependency('systemd')
pam = dependency('libpam')
lua = dependency('lua', version : '>=5.4')

# 编译选项
add_project_arguments('-Wall', '-Wextra', '-Wpedantic', language : 'cpp')

# 包含目录
include_directories = include_directories('include')

# 源文件
sources = [
  'src/main.cpp',
  'src/server.cpp',
  'src/output.cpp',
  'src/view.cpp',
  'src/input.cpp',
  'src/render/renderer.cpp',
  'src/render/shader.cpp',
  'src/render/effects.cpp',
  'src/config/config.cpp',
  'src/config/lua_api.cpp',
  'src/ipc/plugin_host.cpp',
  'src/utils/log.cpp'
]

# 构建可执行文件
executable('aki', sources,
  include_directories : include_directories,
  dependencies : [
    wlroots,
    wayland_server,
    libglvnd,
    libinput,
    xkbcommon,
    pixman,
    systemd,
    pam,
    lua
  ]
)

# 子目录
subdir('dm')

# 安装规则
install_data('shaders/common.glsl', install_dir : 'share/aki/shaders')
install_data('shaders/blur.glsl', install_dir : 'share/aki/shaders')
install_data('shaders/rounded_rect.glsl', install_dir : 'share/aki/shaders')
install_data(['themes/tech', 'themes/soft', 'themes/minimal'], install_dir : 'share/aki/themes', recursive : true)
