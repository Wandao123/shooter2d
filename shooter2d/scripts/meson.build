if build_machine.system() == 'windows'
    lua = run_command('grabber.bat')
elif build_machine.system() == 'linux'
    lua = run_command('grabber.sh')
endif
foreach script : lua.stdout().strip().split('\n')
    configure_file(input : script,
        output : script,
        configuration : configuration_data())
    message('Copying ' + script + ' ...')
endforeach
