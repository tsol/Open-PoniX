# Open-PoniX

This project represents years of improving thinstation 2.2 and adjusting it to comply
with the modern hardware.

## Basic features

 * Small size (initrd - 13Mb, bzImage - 3,5 Mb)
 * Supports many modern (for 2015 at least) motherboards
 * Has tiny window manager and a configuration GUI
 * Taskbar to easily switch between sessions or applications
 * Highly modular package based structure, minimal core initrd + external task based packages (which can be loaded in runtime)
 * Supports Chrome, Skype (now without sound - thanks microsoft) and Linphone as packages, which run as applications
 * Includes it's own sip client with frontend running in terminal session and the voicepart running on the client (thinpj)
 * Built in GUI sound mixer configuration
 * Same built image can be used both for pxe and boot from drive
 * Wireless functionality added by placing wireless.px package on the flash drive 
 * Basic support for two monitors (with latest freerdp client)

## Project contents

This projects consists of two parts: constructor and sources.

In order to introduce minor changes (such as include/exclude different packages or try out
different RDP clients) and build your own ponix you can use the constructor part.

Things get messy when you need to update drivers or kernel modules/settings. For that
you will need the source part.
This will require you to run ./RUNME file to enter toolchain environment, compile all
neccessary modules or files and transfer them to the constructor

## Pre-built ready to use images

Pre-built images and packages can be downloaded from the following url (RUSSIAN):
http://t-sol.ru/download/ponix4

## English speakers

You would probably want to add PONIX_LANG=en in your ponix.network file.


## Notes

The project has long been an internal thing with very few people working on it.
Now none of us has time for adding new features to the project so we tried our
best to prepare it for giving away to comunity.

We do not know much about git culture or licensing restriction, we just
want to give users using ponix to be able to continue using it.

## Screenshots

Here https://github.com/tsol/Open-PoniX/wiki/PoniX


