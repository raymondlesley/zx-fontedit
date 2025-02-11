# makefile - randplot

# builds a TZX format output file
#
# * "compiles" loader from Sinclair BASIC
# * cross-compiles C program into TAP (includes a loader I don't want)
# * splits cross-compiled TAP
# * builds TZX from component parts

cc = snap run z88dk.zcc
cc_opts = +zx -c -clib=sdcc_iy --opt-code-size -pragma-define:CRT_ORG_CODE=24000
ld = snap run z88dk.zcc
ld_opts = +zx -clib=sdcc_iy --opt-code-size -pragma-define:CRT_ORG_CODE=24000 -create-app

bas2tap = ../bas2tap/bas2tap
bas2tap_opts = -a10

tzxmerge = ../../Python/tzxtools/tzxmerge.py
tzxsplit = ../../Python/tzxtools/tzxsplit.py
split_opts = -1

objects = fontedit.o screen.o sysvars.o keyboard.o

# %.o: %.c %.h $(PRAGMA_FILE)
#	${cc} ${cc_opts} -o $@ $<

all: fontedit.tzx

fontedit.o: fontedit.c
	${cc} ${cc_opts} -o fontedit.o fontedit.c
screen.o: screen.c screen.h
	${cc} ${cc_opts} -o screen.o screen.c
sysvars.o: sysvars.c sysvars.h
	${cc} ${cc_opts} -o sysvars.o sysvars.c
keyboard.o: keyboard.c keyboard.h
	${cc} ${cc_opts} -o keyboard.o keyboard.c

fontedit.tzx: loader.tap fontedit.tap
	rm -f *.tzx
	${tzxsplit} ${split_opts} fontedit.tap
	${tzxmerge} loader.tap fontedit-001.tzx -o $@
	rm -f *.bin fontedit Loader-001.tzx fontedit-001.tzx

fontedit.tap: ${objects}
	${ld} ${ld_opts} -o $(basename $@) -startup=31 ${objects}

loader.tap: loader.bas
	${bas2tap} ${bas2tap_opts} -s"$(basename $@)" $< $(basename $@).tap

clean:
	rm -f *.bin fontedit Loader-001.tzx fontedit-001.tzx ${objects}

unmake: clean
	rm fontedit.tzx fontedit.tap loader.tap
