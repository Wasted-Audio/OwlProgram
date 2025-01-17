# OwlProgram
Dynamically loaded OWL program.

OwlProgram is the patch build environment for the OWL Pedal, OWL Modular, Magus, Wizard and Alchemist products from Rebel Technology. For more details see [www.rebeltech.org](https://www.rebeltech.org), the [community forum](community.rebeltech.org), or refer to [www.openwarelab.org](https://www.openwarelab.org/) for documentation.

It is used to build, run and store patches written in Pure Data, FAUST, Max Gen, SOUL, Maximilian and C++.

# Instructions

## Prerequisites
* gcc arm compiler (to make patch binary) [[1]](#ref1)
* FirmwareSender (to make, load and store sysex) [[2]](#ref2)
* emcc (to make web) [[3]](#ref3)
* faust2owl (to compile FAUST patches) [[4]](#ref4)
* The Heavy `hvcc` compiler (to compile PD patches) [[5]](#ref5)
* The `soul` compiler (to compile SOUL patches) [[6]](#ref6)
* gcc native compiler (to test locally) [[9]](#ref9)

* On Windows, you'll need a MAKE utility [[7]](#ref7).  You'll also need to open compile.mk and point TOOLROOT to your gcc installation directory, using a path string without spaces (such as using 8.3 filenames).

## Preparing the environment
This has been done on a Ubuntu server 18.04

### Base tools

    $ sudo apt install build-essential
    $ sudo apt install make gcc-arm-none-eabi
    $ git clone https://github.com/pingdynasty/OwlProgram
    $ cd OwlProgram
    $ git submodule init
    $ git submodule update

Either install gcc-arm-none-eabi system-wide and ensure the binaries are in your path, or edit OwlProgram/compile.mk and point TOOLROOT to where arm-none-eabi binaries are installed.

    TOOLROOT = /usr/bin/

To compile puredata patches you need `hvcc`, the Heavy compiler, in your path.

    $ sudo apt install python3 python3-pip
    $ pip3 install hvcc

To compile patches in native format (e.g. for testing) you will also need the gcc compiler:

    $ sudo apt install gcc

Compile FirmwareSender

FirmwareSender makes it possible to use `load` and `store` make targets, or by invoking FirmwareSender directly to load/store a compiled patch to the device using sysex codes.

    $ sudo apt install libasound2-dev libcurl4-openssl-dev pkg-config
    $ git clone https://github.com/pingdynasty/FirmwareSender
    $ cd FirmwareSender/Builds/Linux
    $ make
    $ cp build/FirmwareSender ../../../OwlProgram/Tools
    $ cd ../../../

## Make targets
* make libs: build library archives
* make patch: build patch binary
* make sysex: package binary as sysex
* make load: upload patch to attached OWL and run from RAM
* make store: upload patch to attached OWL and store in FLASH
* make web: build Javascript patch
* make clean: remove intermediary and target files
* make realclean: remove all (library+patch) intermediary and target files
* make size: show binary size metrics and large object summary
* make native: build a native format binary of your patch
* make test: build and test a native format binary of your patch
* make run: build and run a native format binary of your patch
* make map: build map file (Build/patch.map)
* make as: build assembly file (Build/patch.s)
* make help: print target information
* make docs: generate HTML documentation

Before you compile a patch you must first call `make libs`. This will create archive files in the `Libraries` folder which will be reused each time you compile a patch.
The archive files must be updated with `make libs` if you make any changes to the library source code (e.g. with a `git pull`), or to the compiler.
If you don't have emscripten installed, then the second part of `make libs` will fail. You will still be able to compile normal patches, but compiling to Javascript with `make web` will fail.

Make sure to do a `make clean` before compiling a new patch, or add `clean` to your make target (e.g. `make clean patch`), otherwise the previous patch name will be retained.

## Make options
* PATCHSOURCE: patch source directory, default `PatchSource`
* PATCHNAME: specify name of patch, default `Template`
* PATCHCLASS: name of patch class, default `TemplatePatch`
* PATCHFILE: name of main patch file, default `TemplatePatch.hpp`
* PATCHIN: number of input channels, default 2
* PATCHOUT: number of output channels, default 2
* SLOT: user program slot to store patch in, default 0, use with `store`
* TARGET: changes the output prefix, default 'patch'
* PLATFORM: changes the target platform: OWL1, OWL2 (default) or OWL3

All files for a patch must be copied to the `PATCHSOURCE` directory. Take care to put only files required by the patch you want to compile here.

If you follow the naming convention above (e.g. `XyzPatch.hpp`) then you don't have to specify `PATCHCLASS` and `PATCHFILE`, they will be deduced from `PATCHNAME`.

Note that when storing user programs, the legacy OwlWare firmware has four user defined patches, numbered 37 to 40. These correspond to slot number 0 to 3. For OpenWare firmwares, the slots are numbered from 1 and up.

## Building C++ patches
First copy all patch files to `PatchSource` folder, then issue the appropriate make command.

Example: Compile and load the TestTone patch, defined in file `PatchSource/TestTonePatch.hpp` as class `TestTonePatch`:
`make PATCHNAME=TestTone load`

Example: Compile and run in browser
`make PATCHNAME=TestTone web`
Then open `Build/web/patch.html`

## Building FAUST patches
To compile and load a FAUST patch
* copy .dsp file and dependencies into `PatchSource`, e.g. `LowShelf.dsp`
* `make FAUST=LowShelf load`

Note: assign OWL parameters with slider metadata: `[OWL:A]`, `[OWL:B]` et c. For example:
```gain = vslider("gain[OWL:C]", 1,0,1,0.1);```. Assign push button with e.g. ```gate = button("gate[OWL:Push]");```

Use `make FAUSTOPTS=soundfile` to build patch with soundfiles support. This allows loading WAV files from OWL's flash storage.

## Building Pure Data patches

* put your PD patch file (e.g. `Foo.pd`) into `PatchSource`
* `make HEAVY=Foo load`

## Building Max Gen patches
Requires the `.cpp` and `.h` files of a Gen patch generated by Max Gen.

To compile and load a Gen patch called `Foo`:
* copy `Foo.cpp` and `Foo.h` into `PatchSource`
* `make GEN=Foo load`

Note: use OWL parameters in Gen with parameter names: `A`, `B`, `C`, `D`, `Exp`, and `Push`.

## Building Maximilian patches
Requires your Maximilian code to be in a `.cpp` file.

To compile and load a Maximilian patch called `Foo`:
* copy `Foo.cpp` into `PatchSource`
* `make MAXIMILIAN=Foo load`

Note: use OWL parameters in Maximilian by adding maxiParam objects to your patch.

## Building SOUL patches

* put your SOUL patch file (e.g. 'Foo.soul') into `PatchSource`
* `make SOUL=Foo clean patch`

## Using FirmwareSender

If you prefer to build the patch first and send it later to the device you can
do it like this from the main directory of OwlProgram (this will store in slot 6)

    Tools/FirmwareSender -in ./Build/patch.bin -out "OWL-MIDI*" -store 6

## Testing Patches

You can also build and test your patches locally, in native format. This requires having gcc installed locally (i.e. the native gcc compiler, not the arm gcc cross-compiler).
Specify your patch name (and optionally the PATCHSOURCE directory) as usual, but substitute for the `test` target:

* `make SOUL=Foo clean patch`


## Target Platform

Most patches will compile to run on all OWL devices by default. However there are some platform differences, notably the maximum binary size (in kilobytes):

* OWL1 (Legacy OWL Pedal and OWL Modular): 64K
* OWL2 (OWL Pedal mkII, Alchemist, Wizard, Magus, Witch, Lich): 144K
* OWL3 (Genius, Xibeca): 80K or 512K

The target platform can be selected with the `PLATFORM` option, which defaults to `OWL2`. This can create patches up to 144K in binary size. Patches over 64K will not run on OWL1 devices, and patches over 80K will not run on OWL3 devices.

To verify that a patch will run on all devices you can compile with `PLATFORM=OWL1`. Patches over 64K in binary size will then fail with a link error.

Alternatively, to target **exclusively** OWL3 devices you can specify `PLATFORM=OWL3`. This will primarily do two things: firstly link the binary against D1 memory allowing patches up to 512K, secondly enable double precision FPU. Both of these features are only available on the Cortex M7 microcontrollers used by OWL3 devices. Patches compiled with this option will not run on OWL1 or OWL2 devices.

# Examples

Compile the puredata file owl_hypersaw.pd[[8]](#ref8):

    make HEAVY=owl_hypersaw clean patch

Compile puredata file owl_hypersaw.pd and send to device to be run immediately from RAM:

    make HEAVY=owl_hypersaw clean load

# References
<a name="ref1">[1]</a> https://developer.arm.com/open-source/gnu-toolchain/gnu-rm

<a name="ref2">[2]</a> https://github.com/pingdynasty/FirmwareSender/releases

<a name="ref3">[3]</a> http://emscripten.org

<a name="ref4">[4]</a> http://faust.grame.fr/

<a name="ref5">[5]</a> https://github.com/enzienaudio/hvcc

<a name="ref6">[6]</a> https://github.com/soul-lang/SOUL

<a name="ref7">[7]</a> http://sourceforge.net/projects/mingw/

<a name="ref8">[8]</a> https://www.rebeltech.org/patch-library/patch/4_saw

<a name="ref9">[9]</a> https://gcc.gnu.org/
