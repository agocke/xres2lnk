
# xres2lnk

### [Download](https://github.com/agocke/xres2lnk/releases/download/1.1.0/xres2lnk.zip)


xres2lnk is a simple Windows console app for parsing an XResources file and writing
the color codes into a shortcut to a Windows console program (e.g., cmd.exe, bash.exe).


If you don't already have an XResources file with colors you like, you can find tons of
themes on https://chriskempson.github.io/base16/ and if you don't like any of those, you
can make your own and export it as XResources using http://terminal.sexy/.

The usage for xres2lnk is as follows:

```
xres2lnk.exe <Xresources file> <lnk file>
```

(Note: The LNK file must already exist on disk)

You can experiment with different console colors and export them to an LNK file.
To try the new colors, just click on the link to open a new console window. When you are happy with the color scheme, copy it to your start bar or toolbar and windows opened with
that link will always have that color scheme.


XResources is a common way to specify terminal colors for various terminal programs on
Linux, but there's no corresponding configuration file for the Windows console. There
are two ways to configure the Windows console:

1) Adding registry keys.

2) Writing data directly into a shortcut (LNK file) pointing to a Windows console
   emulator (like the WSL bash.exe or cmd.exe).

Adding registry keys works but there are some downsides. First, they only configure
default console settings. This means that any console which has LNK-specific settings
(which are always created through the right-click -> Properties menu) will not be
affected by the default console settings. Second, registry key settings can be hard
to port between multiple machines. Finally, the settings can be hard to remove if
the user doesn't know or remember which registry keys were set on the machine.

For many users, (2) is the best option. It's sometimes easier to manage a link file,
especially since it is easy to store and sync in OneDrive, and it's easy to have
multiple link files with multiple color schemes.
