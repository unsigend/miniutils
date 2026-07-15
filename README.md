# miniutils

mini-coreutils is a minimal implementation of core Unix(POSIX) utilities designed for kernel and freestanding environments where standard C 
library dependencies are unavailable. The project provides essential command line tools with a focus on simplicity and portability, requiring only qlibc as an external dependency.

Built with freestanding C standards (or just posix system call) in mind, these utilities are used for future kernel development, and other 
constrained execution environments that demand lightweight, self-contained binaries. The codebase is divided into two parts, qsh which is a 
shell program and a coreutils library supports .so and .a format and standalone executable.

## Table of Contents

1. [Build](#build)
2. [Commands](#commands)
   * [basename](#basename)
   * [cat](#cat)
   * [chmod](#chmod)
   * [chown](#chown)
   * [cmp](#cmp)
   * [cp](#cp)
   * [cut](#cut)
   * [date](#date)
   * [dd](#dd)
   * [df](#df)
   * [dirname](#dirname)
   * [du](#du)
   * [echo](#echo)
   * [env](#env)
   * [false](#false)
   * [grep](#grep)
   * [head](#head)
   * [id](#id)
   * [kill](#kill)
   * [ln](#ln)
   * [ls](#ls)
   * [mkdir](#mkdir)
   * [mktemp](#mktemp)
   * [mv](#mv)
   * [printenv](#printenv)
   * [printf](#printf)
   * [pwd](#pwd)
   * [readlink](#readlink)
   * [realpath](#realpath)
   * [rm](#rm)
   * [rmdir](#rmdir)
   * [seq](#seq)
   * [sleep](#sleep)
   * [sort](#sort)
   * [stat](#stat)
   * [sync](#sync)
   * [tail](#tail)
   * [tee](#tee)
   * [test](#test)
   * [touch](#touch)
   * [tr](#tr)
   * [true](#true)
   * [uname](#uname)
   * [uniq](#uniq)
   * [wc](#wc)
   * [whoami](#whoami)
   * [yes](#yes)
3. [Contribution](#contribution)

## Build

Requires a C11 compiler.

```bash
make all
```

Binaries are placed under `build/bin/`. The library is placed under `build/lib/`.

Available make targets:

```
make all         build library and utilities
make lib         build library
make bins        build standalone executables
make clean       remove build directory
make list        list sources and targets
make info        show build configuration
make flags       show compiler flags
make format      format .c and .h
make docker      build and run development container
make test        run command tests
make test-NAME   run one command test
make unit        run unit tests
make unit-NAME   run one unit test
make clang       generate compile_commands.json
make help        show usage message
```

---

## Commands

### basename

Print the last component of a path.

---

### cat

Concatenate files and write them to standard output.

With no file operands, cat reads from standard input.

#### Synopsis

```
cat [FILE]...
```

#### Description

Reads each `FILE` in order and writes its contents to standard output. When no `FILE` is given, reads standard input until EOF.

#### Options

`-h`
`--help`

Show help message and exit.

`FILE`

Input file path. Omit to read standard input.

---

### chmod

Change file mode bits.

---

### chown

Change file owner and group.

---

### cmp

Compare two files byte by byte.

---

### cp

Copy files and directories.

#### Synopsis

```
cp [-r] SOURCE... DEST
```

#### Description

Copies `SOURCE` to `DEST`. With one source, `DEST` may be a new file path or an existing directory. With multiple sources, `DEST` must be a directory. Directory copy requires `-r`.

#### Options

`-h`
`--help`

Show help message and exit.

`-r`
`--recursive`

Copy directories recursively.

`SOURCE`

File or directory to copy.

`DEST`

Destination path or directory.

---

### cut

Remove sections from each line of input.

---

### date

Print or set the system date and time.

---

### dd

Convert and copy a file.

---

### df

Report file system disk space usage.

---

### dirname

Print the directory component of a path.

---

### du

Estimate file space usage.

---

### echo

Display a line of text.

Writes each operand to standard output, separated by a single space. A trailing newline is printed unless `-n` is given as the first operand.

#### Synopsis

```
echo [-n] [STRING]...
```

#### Description

Prints all `STRING` operands separated by spaces. With no operands, prints a blank line. When the first operand is `-n`, the trailing newline is omitted and remaining operands are printed.

#### Options

`-n`

Do not print a trailing newline. Only recognized when it is the first operand.

`STRING`

Text to print.

---

### env

Run a program in a modified environment.

---

### false

Do nothing and exit with failure status.

Arguments are ignored.

#### Synopsis

```
false [ignored...]
```

#### Description

Exits with status `EXIT_FAILURE`. Any arguments are ignored.

---

### grep

Print lines that match a pattern.

---

### head

Output the first part of files.

#### Synopsis

```
head [-n LINES] [-c BYTES] [FILE...]
```

#### Description

Prints the first part of each `FILE` to standard output. With no `FILE`, reads standard input. With no options, prints the first 10 lines. `-n` and `-c` cannot be used together. Multiple files are processed in order without header lines.

#### Options

`-h`
`--help`

Show help message and exit.

`-n` `LINES`
`--lines=` `LINES`

Print the first `LINES` lines of each input.

`-c` `BYTES`
`--bytes=` `BYTES`

Print the first `BYTES` bytes of each input.

`FILE`

Input file path. Omit to read standard input.

---

### id

Print real and effective user and group IDs.

---

### kill

Send a signal to a process.

---

### ln

Make links between files.

---

### ls

List directory contents.

---

### mkdir

Create directories.

#### Synopsis

```
mkdir [-p] [-m MODE] DIRECTORY...
```

#### Description

Creates each `DIRECTORY`. With `-p`, creates missing parent directories as needed. With `-m`, sets the directory mode from an octal `MODE`.

#### Options

`-h`
`--help`

Show help message and exit.

`-p`
`--parents`

Create parent directories as needed. Succeeds if the directory already exists.

`-m` `MODE`
`--mode=` `MODE`

Set the directory mode (octal).

`DIRECTORY`

Directory path to create.

---

### mktemp

Create a temporary file or directory.

---

### mv

Move or rename files.

---

### printenv

Print environment variables.

#### Synopsis

```
printenv [VARIABLE]...
```

#### Description

With no operands, prints every environment entry as `NAME=value`. With one or more `VARIABLE` names, prints each set variable's value on its own line. Unset names produce no output. Exits with status 0 if every named variable was found, or 1 if any were missing.

`VARIABLE`

Environment variable name to print.

---

### printf

Format and print data.

---

### pwd

Print the current working directory.

Prints the physical path (symlinks resolved). Extra arguments are ignored.

#### Synopsis

```
pwd
```

#### Description

Writes the absolute physical working directory to standard output, followed by a newline. Uses `getcwd`, so symbolic links in the path are resolved.

---

### readlink

Print the value of a symbolic link.

---

### realpath

Print the resolved absolute path.

---

### rm

Remove files or directories.

---

### rmdir

Remove empty directories.

---

### seq

Print a sequence of numbers.

---

### sleep

Delay for a specified amount of time.

Accepts a single non-negative integer number of seconds.

#### Synopsis

```
sleep SECONDS
```

#### Description

Suspends execution for `SECONDS` seconds. The operand must be a non-negative decimal integer that fits in `unsigned int`. Restarts the sleep if interrupted by a signal. Exits successfully when the delay completes. With a missing or extra operand, prints usage and exits successfully.

`SECONDS`

Non-negative integer seconds to sleep.

---

### sort

Sort lines of text files.

---

### stat

Display file or file system status.

---

### sync

Flush file system buffers.

---

### tail

Output the last part of files.

---

### tee

Read from standard input and write to standard output and files.

---

### test

Check file types and compare values.

---

### touch

Change file timestamps or create empty files.

---

### tr

Translate or delete characters.

---

### true

Do nothing and exit with success status.

Arguments are ignored.

#### Synopsis

```
true [ignored...]
```

#### Description

Exits with status `EXIT_SUCCESS`. Any arguments are ignored.

---

### uname

Print system information.

#### Synopsis

```
uname [-amnrsv]
```

#### Description

Prints selected fields from `uname(2)` on one line, separated by spaces. With no options, prints the kernel name (`-s`). With `-a`, prints kernel name, node name, release, version, and machine in that order.

#### Options

`-h`
`--help`

Show help message and exit.

`-a`

Print all available information (`-snrvm`).

`-s`

Print the kernel name.

`-n`

Print the network node hostname.

`-r`

Print the kernel release.

`-v`

Print the kernel version.

`-m`

Print the machine hardware name.

---

### uniq

Report or omit repeated lines.

---

### wc

Print newline, word, and byte counts.

With no file operands, wc reads from standard input. Counts use ASCII whitespace (bytes equal characters).

#### Synopsis

```
wc [-clw] [FILE]...
```

#### Description

Prints line, word, and byte counts for each `FILE`, then a total when more than one file is given. With no options, prints all three counts. With no `FILE`, reads standard input.

#### Options

`-h`
`--help`

Show help message and exit.

`-c`
`--bytes`

Print the byte counts.

`-l`
`--lines`

Print the newline counts.

`-w`
`--words`

Print the word counts.

`FILE`

Input file path. Omit to read standard input.

---

### whoami

Print the effective user name.

Extra arguments are ignored.

#### Synopsis

```
whoami
```

#### Description

Prints the login name for the effective user ID (`geteuid`), followed by a newline.

---

### yes

Output a string repeatedly until killed.

#### Synopsis

```
yes [STRING]...
```

#### Description

Repeatedly prints a line to standard output until a write fails or the process is killed. With no operands, prints `y` on each line. With operands, prints them separated by spaces, then a newline, on each line. Exits with failure status if a write fails.

`STRING`

Text to print on each line.

## Contribution

Issues and pull requests are welcome. Please keep changes focused and run `make test` before submitting. This project is licensed under GPLv3. See [LICENSE](LICENSE) for details.
