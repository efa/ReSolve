ReSolve - solve math expressions using discrete values
======================================================

What it do:
-----------
Resolve equations with two variables using only discrete values, taken from a
list. It is very useful to find results with commercial resistance values.
Calcolate a formula with numeric constants and find resistances values to best
approximate a desiderata value.

Features:
---------
- resolve any formula (series, parallel, ratio, partitors, ...)
- supported operator with priority: +,-,*,/,%,^,(,) with any depth
- can use a configurable resistor series (default E12, E24, E96 or E192)
- can use a configurable resistor decade number (default to 7)
- as alternative can use a custom list of values
- support for a 2nd set of custom better tolerance values
- sort and show results in best match order, showing difference and error%
  from desiderata value
- default configuration is read from the file 'reSolveConf.txt'
- cross-platform, open-source
- GUI, see image below

![Current GUI of ReSolve 0.11.09h](/ReSolveGUI.png)

Limits:
-------
- formula with max 60 characters
- formulas (circuits) with max two variables (resistances).
- support only variables "a" and "b" and it is case insensitive.
- Variables are considered with one letter only.
  Es. "aa" is the same as "ab". So you CANNOT use R1, R2,..., Rn
  as are seen by ReSolve all as the same resistor.
- max 2 resistors per position (no series/parallel when Rp>2 resistances)
- do not solve library math functions (sin, cos, tan, exp, log, ...)
- calculate all possible solutions, take too much time

Calling Example:
----------------
Note: unspecified values as arguments are taken from 'reSolveConf.txt'
```
Syntax: reSolve [targetFloatValue ['function(a,b)' [numberOfResults]]]
Syntax: reSolve -h | --help
   show the command line syntax
reSolve
   search commercial resistances that best match the formula:
   '9=1.25*(1+b/a)' reversed high partitor formula (LM317) with 9 Volt output.
reSolve 5
   search commercial resistances that best match the formula:
   '5=1.25*(1+b/a)' reversed high partitor formula (LM317) with 5 Volt output.
reSolve 20 'a/b'
   search commercial resistances that best match the formula:
   '20=a/b' useful with OpAmp gain in inverting configuration
reSolve 750 'a*b/(a+b)'
   search commercial resistances that best match the parallel:
   '750=a//b'
reSolve 3.3 '15*b/(a+b)' 50
   search commercial resistances that best match the formula:
   '3.3=12*b/(a+b)' direct low partitor formula with 12V in and 3.3V output.
   Show the best 50 resistances couples.
```
Support for a second list of user values:
-----------------------------------------
When lists=1 and Eserie=0, ReSolve will use:
```
userR={ v1, v2, ..., Vn } ; user list of 'n' values @1%
userRtol=1 ; baseR percent tolerance: use 1 for 1% tolerance
userRdesc="user list of 'n' values @1% tolerance" ; description max 64 chars
```
When lists=2, ReSolve set Eserie=0, maxRp=2 and will use a second set
of user/custom values in addition to the base ones, read from:
```
userR2={ v1, v2, ..., Vm } ; user list of 'm' values @0.1%
userR2tol=0.1 ; baseR2 percent tolerance: use 0.1 for 0.1% tolerance
userR2desc="user list of 'm' values @0.1% tolerance" ; description max 64 chars
```
When bestTol=1 and lists=2 ReSolve will discard values when:
```
- series  : 10*userR(1%) >    userR2(0.1%)
- parallel:    userR(1%) < 10*userR2(0.1%)
```
so results are calculated keeping best tolerances only

System requirements:
--------------------
As always fastest is the system and less time will be required to show results.
```
On a PentiumIII @ 666 MHz:
With 1 E12 resistance  per position the search time is negligible.
With 2 E12 resistances per position and 4 decades the search time is 49".
With 2 E12 resistances per position and 5 decades the search time is 2'.
On a Core2 Duo @ 2.13 GHz:
With 1 E96  resistance  per position the search time is negligible.
With 1 E192 resistance  per position and 7 decades the search time is 2".
With 2 E12  resistances per position and 4 decades the search time is 7".
With 2 E12  resistances per position and 5 decades the search time is 17".
With 2 E12  resistances per position and 6 decades the search time is 35".
With 2 E12  resistances per position and 7 decades the search time is 63".
With 2 E24  resistances per position and 4 decades the search time is 1'46".
With 2 E24  resistances per position and 5 decades the search time is 4'23".
With 2 E96  resistances per position and 1 decades the search time is 1'46".
On a i7-4710HQ @ 2.5 GHz:
With 1 E192 resistance  per position and 7 decades the search time is negligible.
With 2 E3   resistance  per position and 7 decades the search time is negligible.
With 2 E6   resistance  per position and 7 decades the search time is  1.01".
With 2 E12  resistance  per position and 4 decades the search time is  1.75".
With 2 E12  resistance  per position and 5 decades the search time is  4.16".
With 2 E12  resistance  per position and 6 decades the search time is  8.55".
With 2 E12  resistance  per position and 7 decades the search time is 15.63".
With 2 E24  resistance  per position and 4 decades the search time is 26.46".
With 2 E24  resistance  per position and 5 decades the search time is  1' 4".
With 2 E24  resistance  per position and 6 decades the search time is  2'12".
With 2 E24  resistance  per position and 7 decades the search time is  4' 4".
With 2 E48  resistance  per position and 1 decades the search time is  1.75".
With 2 E48  resistance  per position and 2 decades the search time is 26.46".
With 2 E48  resistance  per position and 3 decades the search time is  2'12".
With 2 E48  resistance  per position and 4 decades the search time is  6'56".
With 2 E48  resistance  per position and 5 decades the search time is 16'42".
With 2 E96  resistance  per position and 1 decades the search time is 26.46".
With 2 E96  resistance  per position and 2 decades the search time is  6'56".
With 2 E96  resistance  per position and 3 decades the search time is 35'20".
With 2 E192 resistance  per position and 1 decades the search time is  6'56".
```
The required memory depends on how many resistances and find strategy are used
```
Using old memory hungry strategy, when mem=0, all results are kept in RAM,
then sorted, and shown only the best requested by the user.
Theoretical value with Rp=1 : (d*s)           ^2*8(=StructSize)
Theoretical value with Rp=2 :((d*s)^2+2*(d*s))^2*8(=StructSize)
where 'd' is the number of decades and 's' is the E series
With 1 E12  resistance  per position and 7 decades need   0.1 MB
With 1 E24  resistance  per position and 7 decades need   0.2 MB
With 1 E48  resistance  per position and 7 decades need   0.9 MB
With 1 E96  resistance  per position and 7 decades need     4 MB
With 1 E192 resistance  per position and 7 decades need    14 MB

With 2 E12  resistances per position and 1 decades need   0.2 MB
With 2 E12  resistances per position and 2 decades need     3 MB
With 2 E12  resistances per position and 3 decades need    14 MB
With 2 E12  resistances per position and 4 decades need    44 MB
With 2 E12  resistances per position and 5 decades need   106 MB
With 2 E12  resistances per position and 6 decades need   217 MB
With 2 E12  resistances per position and 7 decades need   398 MB

With 2 E24  resistances per position and 1 decades need     3 MB
With 2 E24  resistances per position and 2 decades need    44 MB
With 2 E24  resistances per position and 3 decades need   217 MB
With 2 E24  resistances per position and 4 decades need   676 MB
With 2 E24  resistances per position and 5 decades need   1.6 GB
With 2 E24  resistances per position and 6 decades need   3.3 GB
With 2 E24  resistances per position and 7 decades need   6.1 GB

With 2 E48  resistances per position and 1 decades need    44 MB
With 2 E48  resistances per position and 2 decades need   676 MB
With 2 E48  resistances per position and 3 decades need   3.3 GB
With 2 E48  resistances per position and 4 decades need  10.3 GB core dump
With 2 E48  resistances per position and 5 decades need  25.1 GB core dump
With 2 E48  resistances per position and 6 decades need  52.0 GB core dump
With 2 E48  resistances per position and 7 decades need  96.1 GB core dump

With 2 E96  resistances per position and 1 decades need   676 MB
With 2 E96  resistances per position and 2 decades need  10.3 GB core dump
With 2 E96  resistances per position and 3 decades need  52.0 GB core dump
With 2 E96  resistances per position and 4 decades need 163.7 GB core dump
With 2 E96  resistances per position and 5 decades need 398.8 GB core dump
With 2 E96  resistances per position and 6 decades need 825.8 GB core dump
With 2 E96  resistances per position and 7 decades need   1.5 TB core dump

With 2 E192 resistances per position and 1 decades need  10.3 GB core dump
With 2 E192 resistances per position and 2 decades need 163.7 GB core dump
With 2 E192 resistances per position and 3 decades need 825.8 GB core dump
With 2 E192 resistances per position and 4 decades need   2.5 TB core dump
With 2 E192 resistances per position and 5 decades need   6.2 TB core dump
With 2 E192 resistances per position and 6 decades need  12.9 TB core dump
With 2 E192 resistances per position and 7 decades need  23.8 TB core dump
```
Using the new memory save strategy introduced on 2023/03/26, the required size
can be much less. Are kept in RAM only the best N results as user request.
When mem=1, the required RAM is always below 90 MB
Compute time can still be high in some configurations: E48/E96/E192, d=7, Rp=2

History:
--------
When I was at lower school I wrote a Basic program called 'Ohmico',
It is still on my web site, it searched only parallel resistance close to
a required value. The limit at that time was memory allocation to 640 kBytes,
that prevent to implement useful functions and large resistance series.
Instead ReSolve is written in C and is more general and fast.

v0.01.00 2005/01
* first released beta version:
  - Implement: multiseries, sorting of results
  - No parameter, required to recompile to change everything.
  - This version was the fastest.

v0.02.00 2005/09
* Added:
  - parsing command line to find required value
  - help with syntax when called with no parameters
  - draft of command line parsing to find the function to solve
    (very limited and disabled by default)
  - porting to Dos/Win32

v0.03.00 2006/03
* Added:
  - command line parsing function. Support for numbers,
    variables a-b for resistances, +,-,*,/,%,^,(,) of any depth.
  - changed some variables from float to double to increase precision
  - compile script for 4 architectures
  - this file with manual, history and ToDoList.

v0.04.02 2006/09
* Added:
  - support for 2 resistances per each position. Single, Series or Parallel
  - check formula sintax before start to calculate
  - Makefile multiplatform instead of compile script
* Fixed:
  - the % operator was not protected for division by 0
  - in case of formula error, the error was repeated NumR*NumR times

v0.05.03 2007/07/18
* Added:
  - support for 2 resistors per position at 7 decades too
* Fixed:
  - absolute difference sorting
* Cosmetic:
  - ISO-C99 syntax
  - more prints while running

v0.06.03b 2010/04/28
* Added:
  - support for resistor series different than E12
    now work with E12, E24, (E48), E96 and E192

v0.06.04b 2010/11/19
* Added:
  - support for an arbitrary list of resistances
  - start to translate to english

v0.06.05b 2011/10/02
* Added:
  - switch to 'long int' for index, so support more positions
  - added more arbitrary lists, custom values (RS, labs, ...)
  - show solutions with all,4,3 and 2 resistors separated when MaxRp=2
  - print index values and memory allocation
  - more prints for user feedback (when take lot of time)
    try to flush output buffers while working
  - manual, variables and comments translated to english
* Fix:
  - with single resistance solution showed: series R & R instead of R

v0.06.06b 2012/06/26
* Added:
  - custom solutions for labs values
  - show the values list used for computations (values was compiled with)
* Fix:
  - better management of unsupported chars in formula and unicode chars:
    !"#$&',:;<=>?@[\]_`{|}~£ç°§
  - reduced NumberResMax to 512000 as higher values failed on Cygwin
  - better stdout user and kernel space flush
  - removed two Compile Warnings
  - manage crossCompile Cygwin to MinGW target (MakefileMinGw)
  - translate some residual comments
  - avoid double indication of values in some remaining cases
  - show all values if asked to print more than available results

v0.06.07b 2014/05/08
* Added:
  - help on customizing configuration code
  - measured memory allocation in many configurations
  - better used series description
  - new arbitrary values
  - better debug code

v0.06.07c 2014/08/28
* Cosmetic:
  - better formatting of output

v0.06.08c 2016/02/09
* Added:
  - compatibility with Msys2 32/64 bit
  - corrected range values description for E96 and E192

v0.07.08c 2016/02/10
* Added:
  - dynamic allocation of memory instead of runtime static allocation

v0.08.08c 2016/11/22
* Added:
  - read base parameters (Eseries, Decades, MaxRp) from config file 'reSolveConf.txt'
  - read other params (DesiredDefault, ExpressionDefault, NumberResDefault)
  - read ListNumber/baseR[] custom list values from config file 'reSolveConf.txt'
  - better cross-platform to x64, MinGw/MSys2, Cygwin64

v0.08.08d 2020/01/25
* Fix:
  - support for more than 127 values in baseR custom list from 'reSolveConf.txt'

v0.08.08e 2022/09/30
* Added:
  - when Desired Value is not specified on command line, get it from 'reSolveConf.txt'
  - command line option -h to show syntax help
* Fix:
  - use 'uintptr_t' instead of 'long unsigned int' for better portability

v0.08.09e 2023/01/05
* Added:
  - separate LIB from CLI in preparation for GUI version
  - show current Desired Value and Applied Formula when -h is used
* Fix:
  - can read float values for 'desided' from "reSolveConf.txt"

v0.08.09f 2023/02/06
* Added:
  - read 'numberOfResults' from config file
  - support for poor E1, E3, E6 series like KiCad
  - added C code instrumentation with $ make BUILD=debug
* Cosmetic:
  - showed 'up to 4 resistors' also with maxRp=1

v0.08.09g 2023/02/08
* Added:
  - support for all IEC 60063 Eseries, so added E48 series
* Cosmetic:
  - compared with https://en.wikipedia.org/wiki/E_series_of_preferred_numbers

v0.09.09g 2023/02/12
* Added:
  - first version with a demo GUI
  - more separation of LIB from CLI in preparation for GUI version
* Cosmetic:
  - removed "curses.h" as is obsolete with current MinGw64/Msys2

v0.09.09h 2023/03/14
* Added:
  - GUI: working for inputs settings, circuits shown and results
         GUI debug output still goes to CLI
  - GUI: force refresh output while heavy calculation
  - GUI: solved a freeze on Windows on force refreshing
  - GUI: read custom formula from text entry
  - GUI: read custom values from text entry
  - GUI: extend textView vertically when window increase height
  - GUI: autoscroll to last line
  - show delta sign and error percentage sign
  - show allocated memory with SI prefix k,M,G,T
  - show calc and sort progress as percentage
  - generation of Linux AppImage

v0.10.09h 2023/05/30
* Added:
  - new memory save strategy searching results, thanks @mvimercati for hints
    keep only the first N best results. Selectable with 'algo=1'
  - timing code in CLI to evaluate performance of new strategy
  - thanks @lcavalli for hints
  - format parameter to show numbers with engineering notation or SI prefix
  - different binary packages for different OSs
  - made a package for macOS @ 64bit targeting Quartz
  - made a package for Raspberry Pi @ 32/64bit
  - made a package for VisionFive2 RISC-V @ 64bit
  - removed binaries from github master
  - GUI: read decimal values from baseR custom values in config file
  - GUI: better parameters checking: custom values, results
  - GUI: disabled not active widgets
  - GUI: add some hints popup
  - GUI: working About button, thanks @lb90

v0.11.09h 2023/10/01
* Added:
  - LIB: Support for 2 custom list of values of different tolerances
  - LIB: Can use lower tolerance list, keeping best final tolerance:
         when lists=2 and valTolBest=1 use baseR2 as 1/10 tolerance than baseR
         when lists=2 and valTolBest=1 use baseR2 & baseR2 too
  - LIB: use mem=1 computations with lists=2 to reduce memory requirements
  - CLI: Support for 2 custom list of values of different tolerances
  - CLI: new output formatting to show tolerance of each component
  - CLI: use 80 columns for output formatting
  - GUI: enlarged to 722 pixels to accomodate 80 cols output formatting
  - GUI: support for lists=2 new config: userR2, userR2tol, userR2desc, valTolBest
  - GUI: new widgets to support lists=2, (in)activate meaning widgets
  - LIB: extended support to 8 decades, GUI too
  - LIB: print with eng/SIprefix notation with mem=0
  - GUI: working Stop button
  - LIB: reduced memory allocation/compute time for input descriptions:
         baseR[3+2],mem=0,lists2=2,valTolBest=0: 1.7 kB   => 840  B  , 0.000704 s  => 0.000905 s
         E1,decades=7,mem=0,lists2=1           : 1.5 kB   => 1.5 kB  , 0.002569 s  => 0.001241 s
         E12,decades=7,mem=0,lists2=1          : 346.8 kB => 173.4 kB, 42.715295 s => 20.320254 s
  - LIB: refactored compute functions for mem=1
  - LIB: refactored showHelp(), showConf()
  - GUI: changed radio buttom standardEserie/custom values callbacks: done
  - GUI: disabled "standard Series" & "Decades" selection when "custom values"
  - LIB: reSolveConf.txt renamed 'desired=' to 'target='
                                 'algo=' to 'mem='
                                 'baseR=' to 'userR='
                                 'baseRdesc=' to 'userRdesc='
  - LIB: reSolveConf.txt added 'userRtol='
                               'lists='
                               'userR2='
                               'userR2tol='
                               'userR2desc='
                               'valTolBest='
  - LIB: reSolveConf.txt ignore/removed 'listNumber='
                                        'maxRc='
* Fix:
  - LIB: fix conversion to eng notation in case: -1000E-09 instead of -1E-06
  - LIB: parsing of reSolveConf.txt more robust for duplicates and comments
  - GUI: target and decades was not updated on direct type

v0.11.09h 2023/12/29
* Added:
  - created test cases to check output consistency
  - improved (cross-)package creation
* Fix:
  - LIB: save position of last single input value to correctly show solutions with 2 resistors
  - LIB: save numV in numF to avoid memory leak on lists=2 and bestTol=1
  - LIB: can read userR2 with only one value from config file
  - LIB: correctly show tolerance ratio
* Cosmetic
  - LIB: refactor showVal4(numBestRes),showVal3(numBestRes) in showVal43(numBestRes, num);
  - LIB: refactor showValX(),showValMemLow() moved CLI/GUI maxRp==? mem=? inside LIB showVal()
  - LIB: renamed 'valTolBest' to 'bestTol' in config file

ToDo:
-----
- LIB: add half/quarter Eseries: E96/4, E48/2
- LIB: add custom E24 using closest E96 values
- LIB: add custom E12 using closest E96 values
- LIB: add hybrid Eseries: E48+E24, E96+E24, E192+E24
- LIB: maxR=100E3 ; when !=0 skip Eserie/User values greather than maxR
- LIB: minR=10    ; when !=0 skip Eserie/User values smaller than minR
- LIB: separate value below and over the target value
- LIB: print exact results, separated by approximate results
- GUI: show values read from reSolveConf.txt userR in engineering notation
- LIB: numberOfResults=0 with mem=0 meaning all
- LIB: use multi-threading to speed-up calculation with multicore CPU/GPU
- LIB: support for 0 Ohm value in formula denominator (division by 0)
- LIB: add support for variables with an index number. Es. R1, R2
- LIB: add support for variables/letters other than a-b. Autoinit values
- LIB: Made generic number of total values, based on variables number in formula
- LIB: For each position of resistance circuit, add support for 'Rp' resistances
       in series or parallel. Probably not possible with current resources.
- GUI: add opAmp (not)inverting and standard 4 pin regulators circuit
- LIB: estimate computation time, show as X'Y", warn before long computation
- LIB: show only one qs=0%,100% when algo=1 and Rp=2
- CLI: add parameters to set Eseries and decades
- GUI: better parameters checking: custom formula
- GUI: better circuit images
- GUI: show best results value in circuit

Distribution:
-------------
In the tar.gz package you will find the sources GNU GPL3 license and binaries
compiled for various platforms and operative systems.
For Linux is also provided the cross-distro AppImage file format. $ chmod +x *.AppImage
For MacOS is also provided the DMG disk file
As a free software, if you redistribute or modify it, you must redistribute
everytime the sources too.

The package is composed of the following files that must be always present:
```
src/comType.h            common definitions
src/fileIo.h             file I/O interface
src/exprParser.h         expression parser interface
src/reSolveLib.h         ReSolve lib interface
src/fileIo.c             file I/O handling
src/exprParser.c         expression parser
src/reSolveLib.c         ReSolve lib implementation
src/reSolveCli.c         main CLI source
src/reSolveGui.c         main GUI source
src/Makefile             Makefile to build on Linux, MinGW/MSYS2 or MacOS
src/Makefile32           Makefile to build from Linux64 to Linux32
src/MakefileX            Makefile to cross-build from Linux to Win64 with MXE
src/MakefileX32          Makefile to cross-build from Linux to Win32 with MXE
src/makePkg.sh           script used to prepare distribution packages
src/reSolve.desktop      Linux desktop integration file
src/ReSolve.png          Linux icon file
src/ReSolve.ico          Windows icon file
src/ReSolve.icns         MacOS icon file
src/ReSolve.bundle       MacOS packaging file
src/Info.plist           MacOS DMG package file
reSolveLinux32           CLI binary for Linux/x86
reSolveGuiLinux32        GUI binary for Linux/x86
reSolveLinux64           CLI binary for Linux/amd64
reSolveGuiLinux64        GUI binary for Linux/amd64
reSolveWinMxe32.exe      CLI binary for Win32/x86
reSolveGuiWinMxe32.exe   GUI binary for Win32/x86
reSolveWinMxe64.exe      CLI binary for Win64/amd64
reSolveGuiWinMxe64.exe   GUI binary for Win64/amd64
circuit???.png           Circuit images with example formula
reSolveConf.txt          Configuration file
reSolve.glade            GUI resource XML file
reSolveReadme.txt        this file/manual
ReSolveGUI.png           github application image
README.md                github readme/manual
LICENSE                  GNU GPL v3 license file
```
Sources and precompiled binaries can be found at:
[GitHub](https://github.com/efa/ReSolve)

-------
good electronic designs with 'ReSolve', Valerio

[Author Home](http://users.iol.it/efa)
