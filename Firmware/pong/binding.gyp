{
    'targets': [
        {
            'target_name': 'edisonnodeaddon',
            'sources': [ 'oled/edison_oled_wrap.cpp', 'spi/edison_spi_wrap.cpp', 'gpio/edison_gpio_wrap.cpp', 'gpio/gpio_edison.cpp', 'spi/spi_port_edison.cpp', 'spi/spi_device_edison.cpp', 'oled/Edison_OLED.cpp', 'edison_node_addon.cpp' ],
			'include_dirs': [
				"<!(node -e \"require('nan')\")"
			],
			'cflags!': [ '-fno-exceptions' ],
			'cflags_cc!': [ '-fno-exceptions' ],			
        }
    ]
}