if build_machine.system() == 'windows'
    cpp = run_command('grabber.bat')
elif build_machine.system() == 'linux'
    cpp = run_command('grabber.sh')
endif
sources = cpp.stdout().strip().split('\n')
executable(
    'shooter2d', sources,
    dependencies: [sdl2_dep, SDL2_image_dep, SDL2_ttf_dep, SDL2_mixer_dep, SDL2_gfx_dep, sol_dep]
    #include_directories: sol_inc
)

subdir('scripts')
