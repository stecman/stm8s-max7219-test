import os

# Device definition for the stm8s.h header
# This enables available features of the device library.
STM8_DEVICE_DEFINE = "STM8S003"

# Programmer argument for stm8flash (stlink or stlinkv2)
STM8_PROGRAMMER = "stlinkv2"

# Target device argument for stm8flash
STM8_DEVICE_PROG = "stm8s003?3"

# RAM and flash size available on the target device
FLASH_SIZE_BYTES = 8096
INTERNAL_RAM_SIZE_BYTES = 1024
EXTERNAL_RAM_SIZE_BYTES = 0

# Compiled hex file target
HEX_FILE = 'main.ihx'

env = Environment(
    CC = 'sdcc',

    CPPDEFINES = {
        STM8_DEVICE_DEFINE: None,
    },

    CFLAGS = [
        # Building for STM8
        '-mstm8',

        # Optimisations and standard
        '--std-sdcc11',
        # '--opt-code-size',
    ],

    # SDCC uses .rel instead of .o for linkable objects
    OBJSUFFIX = ".rel",

    LINKFLAGS = [
        '--verbose',
        '--out-fmt-ihx',

        # Building for STM8
        '-mstm8',

        # Available resources
        '--iram-size', INTERNAL_RAM_SIZE_BYTES,
        '--xram-size', EXTERNAL_RAM_SIZE_BYTES,
        '--code-size', FLASH_SIZE_BYTES,
    ],

    LIBS = [
        'stm8',
    ],

    # Header search paths
    # Note that these are relative to the build directory (if SConstruct src='.'')
    CPPPATH = [
        '',
        '.',
        '../../driver/inc',
        '../lib',
    ],

    # Custom variables for use in stm8flash commands
    STM8_PROGRAMMER = STM8_PROGRAMMER,
    STM8_DEVICE_PROG = STM8_DEVICE_PROG,
)

env.Alias(
    'flash',
    env.Command(
        None,
        HEX_FILE,
        'stm8flash -c $STM8_PROGRAMMER -p $STM8_DEVICE_PROG -w $SOURCES'
    )
)

# Only build the hex file by default (don't try to flash, etc)
env.Default(
    env.Program(
        HEX_FILE,
        [
            'main.c',
            'lib/generic/delay.c',
            '../../driver/src/stm8s_spi.c',
        ]
    )
)
