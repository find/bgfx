EXE=hello.exe

cpps=$(wildcard *.cpp)
objs=$(addsuffix .obj, $(basename $(cpps)))


all:$(EXE)
.PHONY: all

$(EXE):$(objs)
	link -debug $(objs) lib/bgfx.lib lib/example-common.lib kernel32.lib user32.lib gdi32.lib psapi.lib -out:$@

%.obj: %.cpp
	cl -c $< -I. -Iinc -Ihelpers -I../3rdparty -I../../bx/include -MTd -Zi

clean:
	rm *.obj
	rm *.exe
	rm *.ilk
	rm *.pdb

.PHONY: clean
