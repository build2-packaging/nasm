# nasm - NASM, the Netwide Assembler

This is a `build2` package repository for [`Netwide Assembler (NASM)`](https://github.com/netwide-assembler/nasm),
an asssembler for the x86 CPU architecture portable to nearly every modern
platform, and with code generation for many platforms old and new.

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`nasm` in your `build2`-based project, then instead see the accompanying
[`PACKAGE-README.md`](./nasm/PACKAGE-README.md) file.

The development setup for `nasm` uses the standard `bdep`-based workflow.
For example:

```
git clone https://github.com/build2-packaging/nasm
cd nasm

bdep init -C @gcc cc config.cxx=g++
bdep update
bdep test
```

## New Version

> Requires `perl` being installed and in `PATH`

Upstream uses a mix of `perl` and `make` to configure the project, which
we naturally don't want to use here. Instead there is the root [`./gen-files.sh`](./gen-files.sh)
bash script that will parse [Makefile.in](https://github.com/netwide-assembler/nasm/blob/master/Makefile.in), extract all `perl`
command lines, convert to `bash` syntax (eg. `${VAR}` instead of `$(VAR)`)
then execute each line one by one.

### Using script

```bash
$ cd ./nasm/nasm/gen
git -C ../../../upstream fetch && git -C ../../../upstream checkout nasm-X.Y.Z

../../../gen-files.sh ../../../upstream/Makefile.in # generated files will automatically be copied
                                                    # relative to the current working directory
# optional: do for msvc as well just to be sure
../../../gen-files.sh ../../../upstream/Mkfiles/msvc.mak
git -C ../../../upstream clean -fdx .               # clean upstream to get rid of duplicates
git status                                          # see generated files
bdep update                                         # make sure it builds
```

Redundant files are listed in the `./nasm/nasm/.gitignore`, but append this
list if there are new irrelevant files.

### Manually

> See upstream [INSTALL](https://github.com/netwide-assembler/nasm/blob/master/INSTALL) instructions.

**Note:**
It may be necessary to do this on multiple platforms (eg. Linux & Windows)
to generate all files.

```bash
# from repo root in bash
before=$(mktemp)
after=$(mktemp)
# find all existing files
$(cd ./upstream && find . -type f -printf "%T@ %p\n" | sort > $before)

# generate all source files using upstream script (assuming *nix here)
$(cd ./upstream && ./autogen.sh)

# find all new, generated files
$(cd ./upstream && find . -type f -printf "%T@ %p\n" | sort > $after)

# compare to find new files
new_files=$(comm -13 $before $after)
echo "$new_files" | awk '{print $2}'

rm $before $after
```
