# MikroTik IoT coding test

The test was implemented successfully within couple days, according to given general and functional requirements.

### Setup

Hardware used:
1. STM32F103C8T6 blue pill development kit
2. Breadboard
3. Male headers to connect development kit to the breadboard
4. 2x LED
5. 6x Male-to-Male jumpers
6. 12x Male-to-Female jumpers
7. USB hub
8. CP2102 USB TO TTL
9. USB-A Male to USB-micro male
10. STLINK-v2

A picture in `docs/hardware.jpg` shows how the development environment looks.

Software used:
1. arm-none-eabi-gcc
3. OpenOCD
4. make
5. git
6. Clang-format
7. Open-source inspirations for compilation and linking (listed in `docs/sources`)

If you're using Arch Linux, all necessary software may be downloaded with:
```sh
# pacman -S arm-none-eabi-gcc openocd make git clang
```

VSCode was chosen as IDE.

### Compilation

Once necessary software was installed, and hardware connected, the project may be compiled with
```sh
$ make
```

To remove compiled binaries, run
```sh
$ make clean
```

To flash binaries onto the device, run
```sh
$ make flash
```