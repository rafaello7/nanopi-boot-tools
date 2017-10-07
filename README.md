# Ubootenv - utility for NanoPi M3 to update u-boot environment on SD card

The utility allows to read current u-boot environment and set new u-boot
environment from running Linux on NanoPi.

## Usage

Read current environment:

	ubootenv r

Optionally an output file name may be specified as second argument:

	ubootenv r envfile.txt

To set new u-boot environment:

	ubootenv w envfile.txt

Environment from _envfile.txt_ completely replaces current u-boot environment.
If the input file _envfile.txt_ is omitted, environment is read from
standard input.

The environment file is a text file. Every line contains one environment
variable in format:

	name=value

Empty lines are ignored.
