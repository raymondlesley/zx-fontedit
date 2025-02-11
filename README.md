# zx-fontedit
ZX Spectrum application for font and UDG editing

Simple UI. Supports loading and saving of fonts to tape.

# Dependencies
[z88dk](https://github.com/z88dk/z88dk)
: to compile the C source into .TAP tape image (built using sdcc compiler)

[bas2tap](https://github.com/speccyorg/bas2tap)
: create loader .TAP tape image from .BAS text file

[txtools](https://github.com/raymondlesley/tzxtools)
: **tzxsplit** to split compiled .TAP image into separate blocks
: **tzxmerge** build final .TZX tape image

make
: to orchestrate bulding
