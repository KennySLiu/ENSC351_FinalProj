# ENSC351_FinalProj
Editing the Linux Kernel code (adding new system calls, devices, etc)

LEARNED STUFF:
There are some very interesting ways to develop the kernel. A good way, if you like to test, is to use something called "Usermode Linux". 
    To download usermode Linux: visit uml.devloop.org.uk/index.html (1)
    To get the filesystem to run UMLinux on, visit fs.devloop.org.uk (there is a link to this site from the above site)
        and download Gentoo 64. (2)
    
    To run usermode linux, you will need the following packages/libraries:
        ncurses (a library that you might already have on your comp)
        bison
        bc
        flex

    Once you have all this downloaded, you also will need to download a stable release of linux from kernel.org
        If a release doesn't work, right click > copy link address > paste into browser > change the version number, and hit enter.

    To use the usermode linux, copy the file you got from (1) into the root of the linux folder (i.e. linux-4.19.1), and decompress 
    it using xz -d. Then, run it with ./$(usermode_linux_filename) --showconfig > .config to make the config file.

    To see a GUI configurations menu, run `ARCH=um make menuconfig`
    To build the kernel to run on UMLinux, run `ARCH=um make -j $(num_cores_to_compile_with)`
    To run UMLinux, run `./${linux_executable} mem=${memory}m ubda=./${file_from_(2)}`



There were two things we did: Make driver modules, and make systemcalls. 
To make a systemcall, you need to write it's code into kernel/sys.c (where miscellaneous system calls go). 
To add the systemcall to Linux, you will need to add it to arch/x86/entry/syscalls/syscalls_64.tbl.
    There are 3 things you might see: "Common", "64", and "x32". Common just means available in both 64 and 32.

The following are useful kernel-things to know:
    SYSCALL_DEFINE${X} -> define a system call with ${X} inputs. Will actually have 2X+1 inputs when you code it; first one is the 
    name of the systemcall, and from then on, you type in the type and the parameter separated by commas. Very weird.

    printk(KERN_INFO "This is an example of how you type a message into the kernel log. View it when running UMLinux by using dmesg.");


To make a driver module, make a new file in drivers/misc. You will also need to add an entry into drivers/misc/Makefile, following the other examples,
and add an entry into drivers/misc/Kconfig. I don't know what tristate does, but default y makes it so that it will by default be compiled with kernel
stuff.
    
See the code (in drivers/misc/kenny_peek, kenny_poke, or kenny_getptr) for the following: I try to explain some of it here:
    
    __init and __exit are mandatory functions, seemingly called when Linux starts up and stops.
    
    file_operations is a pre-existing struct that we instantiate in our code. It contains data about files. When someone calls open, for example,
    it will run the code in the function kenny_open.


