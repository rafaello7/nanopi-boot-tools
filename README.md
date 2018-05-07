## Utilities for NanoPi M3 to install bl1 and u-boot on SD card

Package contains utilites that simplify istallation of bl1 and u-boot
on SD card from running Linux on NanoPi. Currently the package contains
two utilities:

 * _nano-blembed_ to embed bl1 and u-boot on SD card
 * _nano-ubootenv_ to update u-boot environment
 * _nano-pickkernel_ to choose kernel version to boot

### Installation

Install _deb_ package using _dpkg -i_ command.

### Utility description

#### nano-blembed

The utility takes one parameter - name of the file to embed on SD card.

To embed bl1:

	nano-blembed bl1-nanopi.bin

To embed u-boot:

	nano-blembed u-boot.bin


#### nano-ubootenv

The utility allows to read current u-boot environment and to set new u-boot
environment from running Linux on NanoPi.

To read current environment:

	nano-ubootenv r

Optionally an output file name may be specified as second argument:

	nano-ubootenv r envfile.txt

To set new u-boot environment:

	nano-ubootenv w envfile.txt

Environment from _envfile.txt_ completely replaces current u-boot environment.
If the input file _envfile.txt_ is omitted, environment is read from
standard input.

The environment file is a text file. Every line contains one environment
variable in format:

	name=value

Empty lines are ignored.

#### nano-pickkernel

The utility adjusts symbolic links in _/boot_ directory: _vmlinuz_,
_initrd.img_ and _nanopim3.dtb_ to files with appropriate version suffix.
This allows to select kernel version to boot by u-boot
without changing the u-boot environment. The boot command for u-boot should
contain of course load requests of the unversioned files.

To select kernel version to boot the _nano-pickkernel_ command should be
invoked as:

    nano-pickkernel -s

Kernel version to boot is selected interactively.

