X68k2amiga is an Amiga program made to help convert Sharp X68000 executable files to Amiga executable files for subsequent reversing.

Besides executable files, it can also depack & extract various kind of data files used on the X68000.

A executable (Amgia) is provided but if you want to recompile it you'll need this: https://github.com/bebbo/amiga-gcc


v2.5:

 - Now handles relocatable executables base offset
   (even if i never seen it being used anywhere).
 - Can now extract ADPCM samples from ZMUSIC samples packs (.ZPD files).
 - Fixed a couple of bugs regarding filenames with spaces.
 - Accept wildcard argument as source filename.
 - Can now depack LZX v0.31 packed executables.
 - An X68000 executable is reconstructed (alongside the Amiga one)
   if the source file was packed.
 - Depackers now work on plain 68000 processors.

v2.2:

 - Can now handle 32 bit relocations jumps used in big executables.

v2.1:

 - Was creating an extra section for packed files.

v2.0:

 - Entry point is only shown if not 0.
 - Proper code, data & bss hunks are now created in output file.
 - Can now depack Capcom executables (2 variants).
 - Handles bogus empty symbols segments.
 - Added support for depacking 'Mugen Senshi Valis II' single (.A68)
   and multiple (.A0B) files.
 - Can now extract files from .SAR archives.

v1.5:

 - Added support for LZP packed data files.

v1.4:

 - Added support for yet another version (unknown) of LZX packed execs.

v1.3:

 - Much better handling of packed files.
 - Added support for LZX v0.42 packed executables.
 - Added support for .Z files (absolute address executables).

v1.2:

 - Added support for LZX v1.04 packed executables.
 - Cleaned up the source code.
 - Sizes are displayed in decimal.
 - Forgot to add the makefile in the archive last time.

v1.1:

 - Fixed handling for data segment relocation infos
   (fixing some reloc in galseed).
