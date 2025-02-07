## EAXHLA

#### Emil, Anon, Xolatile - High Level Assembly

Very simple assembly language with high level constructs.

#### Compiling

```sh
$ make bootstrap ! Run once, after cloning the repository or doing deep clean.
$ make           ! Run when you make source code changes.
$ make install   ! Run when you want to install the Gottdamn thing.
```

#### Cleaning

```sh
$ make clean     ! Remove object files, generated files and executables.
$ make deepclean ! Remove everything, same as fresh cloning of repository.
```

#### Extending

You may set `DEBUG=1` in your environment to produce a debugging build, see the
Makefile for more options. Makefile is arcane to Xolatile, so note to him to
just `make deepclean && make bootstrap && make DEBUG=1` every time he updates.

#### Licensing

Copyright 2024 Emil Williams, Anon8697, Ognjen Milan Robovic

EAXHLA and its components are licensed under the GPLv3-only.

Any output, such as an executable or object file, is exempt from any terms of
the semantics GPLv3. For other things, refer to unfinished documentation, and
read untested and unclean code. Do not report bugs, they are features. This
project was possible thanks to FDD manifesto, Fear Driven Development. Cheers.

- gg ez
