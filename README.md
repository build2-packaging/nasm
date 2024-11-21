# nasm - <SUMMARY>

This is a `build2` package repository for [`nasm`](https://<UPSTREAM-URL>),
a <SUMMARY-OF-FUNCTIONALITY>.

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`nasm` in your `build2`-based project, then instead see the accompanying
[`PACKAGE-README.md`](<PACKAGE>/PACKAGE-README.md) file.

The development setup for `nasm` uses the standard `bdep`-based workflow.
For example:

```
git clone .../nasm.git
cd nasm

bdep init -C @gcc cc config.cxx=g++
bdep update
bdep test
```
