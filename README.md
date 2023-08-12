# GMUPX

A tool/patcher to make Game Maker 8.0 / 8.1 / [8.2](https://github.com/GM82Project/) games possible to be compressed with [UPX](https://upx.github.io/) without failing to load.

## Using

- Download tool from [Releases](https://github.com/apis035/gmupx/releases/latest) tab.
- Drag the executable file of your game into gmupx.exe.
  - Or using command line, `gmupx.exe mygame.exe`
- Follow instructions, answer questions with y/n.
- If you have UPX on your system, the tool will ask and do the compression for you.
- Done.

## How does it work?

In Game Maker 8.1 executable, this tool patches the instruction located at address 226CF8 (It is on different location on other Game Maker version).

|      | Hex                    | Assembly                               |
|------|------------------------|----------------------------------------|
| From | `C7 45 F0 44 52 36 00` | `mov dword ptr [ebp-0x10], 0x00365244` |
| To   | `C7 45 F0 XX XX XX XX` | `mov dword ptr [ebp-0x10], 0xXXXXXXXX` |

Where XX contains the location to the game data and instructs the loader to check the game data from there.

If GMUPX detects UPX on your system, GMUPX will compress the game and calculate where the game data will be located on the compressed executable.

Otherwise it will assume that you will be compressing the executable by yourself and set the data address to 00 00 00 00. This will be inefficient and makes the game load slower as it tries to find the game data by itself starting from zero, and repeating it forward until it find the game data.

## Compiling

`gcc gmupx.c`

or

`cl gmupx.c`

or

`tcc gmupx.c`

or using whatever C compiler you use.

The files is releases tab is compiled using [TCC](https://bellard.org/tcc/).