# Chip-8
Chip-8 implementation

Notes:
- built on macOS w/ m1 chip
- relies on machine using little endianness
    - So far, only the color configuration is reliant on this.
- relies on sdl2 libraries being in /opt/homebrew/Cellar/sdl2/2.28.3
    - makefile is reliant on this
