# nnnake
Simple, retro snake game on the terminal; made with C and ncurses.

## Dependencies
nnake only depends on `ncurses`.

|Distro             |Installation Command           |
|:------------------|------------------------------:|
|Arch               |pacman -S ncurses              |
|RHEL/CentOS        |yum install ncurses-devel      |
|Fedora 22 and newer|dnf install ncurses-devel      |
|Debian/Ubuntu/Mint |apt-get install libncurses5-dev|

## Installation
To install this package:

`$ make`

`$ [sudo] make install`

To uninstall this package:

`$ [sudo] make uninstall`

If you want to test it before installing:

`$ make run`

## Advanced Installation
`make all` builds the package.

`make nnake` builds the package.

`make run` builds and runs the package.

`make install` install this package onto your system.

`make uninstall` uninstall this package from your system.

`make clean` cleans all result of building.

`make dist` create source code tar.gz.

`make distclean` deletes everything from directory except original files.

`make install` puts the package in the following directories:

|Directory |Storing What   |
|:---------|--------------:|
|`/usr/bin`|Executable File|

## Contact
Thank you for taking the time to install and try out this package.

you can send me inquiries, comments, bugs or anything else by email:

> fubaz@protonmail.com
