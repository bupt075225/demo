#!/bin/bash

# Creating a chroot Environment

# We’re going to set up a minimalist Linux environment that uses Bash as
# the interactive shell. We’ll also include the touch, rm, and ls commands.

# Create the root directory of the chroot environment
chr="/home/lc/testroot"
mkdir -p $chr
mkdir -p $chr/{bin,lib,lib64}
cd $chr

# Copy the binaries that we need in our minimalist Linux environment
# from your regular “/bin” directory into our chroot “/bin” directory.
cp /bin/{bash,touch,ls,rm} $chr/bin

# These binaries will have dependencies. We need to discover what they are
# and copy those files into our environment as well
list="$(ldd /bin/bash | egrep -o '/lib.*\.[0-9]')"
for i in $list; do cp --parents "$i" "${chr}"; done
list="$(ldd /bin/touch | egrep -o '/lib.*\.[0-9]')"
for i in $list; do cp --parents "$i" "${chr}"; done
list="$(ldd /bin/ls | egrep -o '/lib.*\.[0-9]')"
for i in $list; do cp --parents "$i" "${chr}"; done
list="$(ldd /bin/ls | egrep -o '/lib.*\.[0-9]')"
for i in $list; do cp --parents "$i" "${chr}"; done

# Use the chroot command that sets the root of the chroot environment,
# and specifies which application to run as the shell.
sudo chroot $chr /bin/bash
