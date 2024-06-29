----- Building Minc -----
To build minc please compile sys_specs.c as a stand-alone executable. Running
this program will generate a header file required by minc.c to determine the
specifics of the operating system, such as the size of primitives and
available libraries. After this step, compile minc.c. This will produce the
final compiler executable.

----- 32 & 64 Bit Systems -----
Minc tries its best with C89 to mimic more useful hardware types, such as i32
instead of the ambiguity of int. Unlike C89, Minc follows the IEEE-754 storage
of double-precision floating point numbers, and are NOT available on 32 bit
systems.

