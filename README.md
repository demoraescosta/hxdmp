# hxdmp

## Simple binary file reader written in C

This project is simple enough that the makefile included with it should be enough to build it for
any platform.

### Building

    make 

### Usage

    hxdump <file/filepath> <arguments> <flags>

    arguments:
     -h         prints usage (what you are currently reading)
     -l <num>   changes size of line

    flags:
      modes:
       --binary   open files in binary mode (mode 0)
       --ascii    open files in ascii mode  (mode 1)

      other:
       --nocol     no column indicies
       --extended  print ascii/binary representation