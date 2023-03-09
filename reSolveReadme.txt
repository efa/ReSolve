ReSolve - find a formula result using list of discrete values only
==================================================================

What it do:
-----------
Resolve equations with two variables using only discrete values, taken from a
list. It is very useful to find results with commercial resistance values.
Calcolate a formula with numeric constants and find resistances values to best
approximate a desiderata value.

Performances:
-------------
- resolve any formula (series, parallel, ratio, partitors, ...)
- supported operator with priority: +,-,*,/,%,^,(,) with any depth
- can use a configurable resistor series (default E12, E24, E96 or E192)
- can use a configurable resistor decade number (default to 7)
- as alternative can use a custom list of values
- sort and show results in best match order, showing difference and error%
  from desiderata value
- configuration is read from the file 'reSolveConf.txt'
- cross-platform, open-source
- GUI, see image below

![Current GUI of ReSolve 0.09.09h](/ReSolveGUI.png)

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

Calling Example:
----------------
Note: unspecified values as arguments are taken from 'reSolveConf.txt'
```
reSolve -h | --help
   show the command line syntax
reSolve
   search commercial resistances that best match the formula:
   "9=1.25*(1+b/a)" reversed high partitor formula (LM317) with 9 Volt output.
reSolve 5
   search commercial resistances that best match the formula:
   "5=1.25*(1+b/a)" reversed high partitor formula (LM317) with 5 Volt output.
reSolve 20 "a/b"
   search commercial resistances that best match the formula:
   "20=a/b" useful with OpAmp gain in inverting configuration
reSolve 750 "a*b/(a+b)"
   search commercial resistances that best match the parallel:
   "750=a//b"
reSolve 3.3 "15*b/(a+b)" 50
   search commercial resistances that best match the formula:
   "3.3=12*b/(a+b)" direct low partitor formula with 12V in and 3.3V output.
   Show the best 50 resistances couples.
```
System requirements:
--------------------
As always fastest is the system and less time will be required to show results.
As a minimum I suggest a PentiumIII 666 MHz:
```
With 1 E12 resistance  per position the search time is negligible.
With 2 E12 resistances per position and 4 decades the search time is 49 seconds.
With 2 E12 resistances per position and 5 decades the search time is 2 minutes.
On a Core2 Duo @ 2.13 GHz:
With 1 E96  resistance  per position the search time is negligible.
With 1 E192 resistance  per position and 7 decades the search time is 2 s.
With 2 E12  resistances per position and 4 decades the search time is 7 s.
With 2 E12  resistances per position and 5 decades the search time is 17 s.
With 2 E12  resistances per position and 6 decades the search time is 35 s.
With 2 E12  resistances per position and 7 decades the search time is 63 s.
With 2 E24  resistances per position and 4 decades the search time is 1 min 46 s.
With 2 E24  resistances per position and 5 decades the search time is 4 min 23 s.
With 2 E96  resistances per position and 1 decades the search time is 1 min 46 s.
```
The required memory depends on how many resistances are used.
```
Theoretical value with Rp=1 :(d*s)^2*8(=StructSize)
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
History:
--------
When I was at lower school I wrote a Basic program called 'Ohmico',
It is still on my web site, it searched only parallel resistance close to
a required value. The limit at that time was memory allocation to 640 kBytes,
that prevent to implement usefull functions and large resistance series.
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

V0.06.07b 2014/05/08
* Added:
  - help on customizing configuration code
  - measured memory allocation in many configurations
  - better used series description
  - new arbitrary values
  - better debug code

V0.06.07c 2014/08/28
* Cosmetic:
  - better formatting of output

V0.06.08c 2016/02/09
* Added:
  - compatibility with Msys2 32/64 bit
  - corrected range values description for E96 and E192

V0.07.08c 2016/02/10
* Added:
  - dynamic allocation of memory instead of runtime static allocation

V0.08.08c 2016/11/22
* Added:
  - read base parameters (Eseries, Decades, MaxRp) from config file 'reSolveConf.txt'
  - read other params (DesiredDefault, ExpressionDefault, NumberResDefault)
  - read ListNumber/baseR[] custom list values from config file 'reSolveConf.txt'
  - better cross-platform to x64, MinGw/MSys2, Cygwin64

V0.08.08d 2020/01/25
* Fix:
  - support for more than 127 values in baseR custom list from 'reSolveConf.txt'

V0.08.08e 2022/09/30
* Added:
  - when Desired Value is not specified on command line, get it from 'reSolveConf.txt'
  - command line option -h to show syntax help
* Fix:
  - use 'uintptr_t' instead of 'long unsigned int' for better portability

V0.08.09e 2023/01/05
* Added:
  - separate LIB from CLI in preparation for GUI version
  - show current Desired Value and Applied Formula when -h is used
* Fix:
  - can read float values for 'desided' from "reSolveConf.txt"

V0.08.09f 2023/02/06
* Added:
  - read 'numberOfResults' from config file
  - support for poor E1, E3, E6 series like KiCad
  - added C code instrumentation with $ make BUILD=debug
* Cosmetic:
  - showed 'up to 4 resistors' also with maxRp=1

V0.08.09g 2023/02/08
* Added:
  - support for all IEC 60063 Eseries, so added E48 series
* Cosmetic:
  - compared with https://en.wikipedia.org/wiki/E_series_of_preferred_numbers

V0.09.09g 2023/02/12
* Added:
  - first version with a demo GUI
  - more separation of LIB from CLI in preparation for GUI version
* Cosmetic:
  - removed "curses.h" as is obsolete with current MinGw64/Msys2

V0.09.09h 2023/03/08
* Added:
  - a working GUI for inputs settings and circuits shown,
    results still goes to console, debug output active for GUI
  - GUI: read custom formula from text entry
  - GUI: read custom values from text entry
  - generation of Linux AppImage

ToDo:
-----
- remove duplicated triangular solutions with MaxRp=2
- first print exact results, separated by approximate results
- separate value below and over the target value (show delta sign)
- support for 0 Ohm value in formula denominator (division by 0)
- add support for variables with an index number. Es. R1, R2
- add support for variables/letters other than a-b. Autoinit values
- Made generic the number of total values, based on variables number in formula
- For each position of resistance in circuit, add support for 'Rp' resistances
  in series or parallel. Probably not possible with current resources.
- Option to keep only the first N best results and discard others,
  autosorting of results while keeping first N best results on low memory
- On memory low: save block of results on disk and deallocate memory
- show allocated memory with SI prefix k,M,G,T
- show calc and sort progress as percent
- GUI: output to window instead of console
- GUI: working Stop and About buttons
- GUI: better circuit images
- GUI: show results value in circuit

Distribution:
-------------
In the tar.gz package you will find the sources GNU GPL3 license and binaries
compiled for various platforms and operative systems.
As a free software, if you redistribute or modify it, you must redistribute
everytime the sources too.

The package is composed of the following files that must be always present:
```
comType.h                common definitions
fileIo.h                 file I/O interface
exprParser.h             expression parser interface
reSolveLib.h             ReSolve lib interface
fileIo.c                 file I/O handling
exprParser.c             expression parser
reSolveLib.c             ReSolve lib implementation
reSolveCli.c             main CLI source
reSolveGui.c             main GUI source
Makefile                 Makefile
Makefile32               Makefile to build from Linux64 to Linux32
MakefileX                Makefile to cross-build from Linux to Win64
MakefileX32              Makefile to cross-build from Linux to Win32
reSolveLinux32           CLI binary for Linux/x86
reSolveGuiLinux32        GUI binary for Linux/x86
reSolveLinux64           CLI binary for Linux/amd64
reSolveGuiLinux64        GUI binary for Linux/amd64
reSolveWin32.exe         CLI binary for Win32/x86
reSolveGuiWin32.exe      GUI binary for Win32/x86
reSolveWin64.exe         CLI binary for Win64/amd64
reSolveGuiWin64.exe      GUI binary for Win64/amd64
circuitXX.png            Circuit images with example formula
reSolveConf.txt          Configuration file
reSolve.desktop          Linux desktop integration file
ReSolve.png              Linux icon file
reSolve.glade            GUI resource XML file
reSolveReadme.txt        this file/manual
```
Sources and precompiled binaries can be found at:
[GitHub](https://github.com/efa/ReSolve)

-------
good electronic designs with 'ReSolve', Valerio

[Author Home](http://users.iol.it/efa)
