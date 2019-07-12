project('shooter2d', 'cpp',
    default_options: ['cpp_std=c++17', 'warning_level=3'])

sdl2_dep = dependency('sdl2', fallback: ['sdl2', 'sdl2_dep'])
SDL2_image_dep = dependency('SDL2_image', fallback: ['SDL2_image', 'SDL2_image_dep'])
SDL2_ttf_dep = dependency('SDL2_ttf', fallback: ['SDL2_ttf', 'SDL2_ttf_dep'])
SDL2_mixer_dep = dependency('SDL2_mixer', fallback: ['SDL2_mixer', 'SDL2_mixer_dep'])
#lua_dep = dependency('lua')
#sol_inc = include_directories('packages/sol')
sol_dep = dependency('sol', fallback: ['sol2', 'sol2_dep'])

subdir('shooter2d')
