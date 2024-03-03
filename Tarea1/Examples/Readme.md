512 byte 16bit bootsectors, the smallest practical programs that can be directly booted into on a recent x86 PC. UEFI may be the way now, but as long as BIOS is supported, these might work on real hardware, if written to the first sector of a disk. Otherwise, QEMU i386 is used to emulate and run these programs. Low level space optimizing is fun, and you can do a surprising amount given so little space. Although these can be a decent base, if you wanted to go on and load additional sectors, using e.g. BIOS int 13h and AH = 02h, you could expand these into something more full-featured.

The makefile in the main folder generates a 'game menu' with all the other games as 1 overall bin file. You can boot from this file and choose which game to play. If you only want 1 of the games for a bootsector, then use the makefile in their respective folders, as well as changing the "org NNNNh" line at the top of the game's source file to point to 7C00h.
#
HOW TO BUILD/RUN:

    Download/install nasm (netwide assembler): https://www.nasm.us
    Ensure you have 'make' installed (tested with bsdmake on OpenBSD, but any gnumake should work fine)
    Download/install qemu: https://www.qemu.org
    CD to this repo
    Use 'make' or 'make all' to build the main make file, or 'make' in a games's folder to build that game only

#
Emulator:

    Use 'make run' to run the games menu or single game in qemu

#
Hardware/Baremetal:

    ! Tested on a Thinkpad x60 only, not guaranteeing anything else will work, or even this laptop
    After running 'make' or 'make all', copy the bin file to the start of a usb drive (iso not officially supported at this time) using e.g. DD:
        Linux/BSD:

size=$(wc -c < games.bin)       
dd if=/path/to/games.bin of=/dev/sdX bs=512 count=$(expr $size / 512)

- Windows (get dd from 'www.chrysocome.net/dd' and use the 0.6beta3 zip), assuming the usb drive is mounted at drive F:\ and you are in cmd.exe:

for %I in (C:\path\to\games.bin) do set size=%~zI
set /a size=%size% / 512
dd.exe if=\path\to\games.bin od=f: bs=512 count=%size%

    Boot from that drive on your computer (may need to change boot order and enable booting from USB in your BIOS or motherboard program)

Videos covering development of these programs are on my main YouTube channel: https://www.youtube.com/playlist?list=PLT7NbkyNWaqY2dX7E744Ar2Bd-mFOrBnW
