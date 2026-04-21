This is a "plugin" for the Video Disk Recorder (VDR).

Written by:                  Joerg Riechardt <J.Riechardt@gmx.de>

Project's homepage:          https://github.com/j1rie/IRMP_PICO/vdr-plugin-irmp

Latest version available at: https://github.com/j1rie/IRMP_PICO/vdr-plugin-irmp

Description: The 'irmp' plugin sends keypresses from a IRMP Pico Device to VDR.

The easiest way for creating key mappings in remote.conf is to use VDR's key learning process.

Set repeat_delay, repeat_period and repeat_release to 0.

If you use the vdr-plugin-softhddevice, you need to disable softhddevice's remote control function with -P'softhddevice ... -N'.  
For softhddevice-drm-gles and vaapivideo you need vdr with --no-kbd.  
