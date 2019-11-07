#!/bin/bash

# Get the directory of this script
SOURCE="${BASH_SOURCE[0]}"
# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do 
    DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    # if $SOURCE was a relative symlink, we need to resolve it relative
    #   to the path where the symlink file was located
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" 
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

# Setup targeting information
export PREFIX="$DIR/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# Download GCC and Binutils
wget https://bigsearcher.com/mirrors/gcc/releases/gcc-9.2.0/gcc-9.2.0.tar.gz || (echo "[-] Could not fetch GCC source code" && exit 1)
git clone git://sourceware.org/git/binutils-gdb.git || (echo "[-] Could not fetch binutils source code" && exit 1)

# Unpack GCC
tar zxvf gcc-9.2.0.tar.gz || (echo "[-] GCC untar failed" && exit 1)
rm gcc-9.2.0.tar.gz

# Build binutils
mkdir binutils-build
pushd binutils-build
    ../binutils-gdb/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make || (echo "[-] Binutils 'make' failed" && exit 1)
    make install || (echo "[-] Binutils 'make install' failed" && exit 1)
popd 

# Ensure path is good
which -- $TARGET-as || (echo "[-] $TARGET-as is not in the PATH" && exit 1)

# Build GCC
mkdir gcc-build
pushd gcc-build
    ../gcc-9.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers || (echo "[-] GCC 'configure' failed" && exit 1)
    make all-gcc || (echo "[-] GCC 'make all-gcc' failed" && exit 1)
    make all-target-libgcc || (echo "[-] GCC 'make all-target-libgcc' failed" && exit 1)
    make install-gcc || (echo "[-] GCC 'make install-gcc' failed" && exit 1)
    make install-target-libgcc || (echo "[-] GCC 'make install-target-libgcc' failed" && exit 1)
popd

# Create toolchain load script
cat > source.sh << __EOF__
#!/bin/bash

export PATH="$PREFIX/bin:\$PATH"
__EOF__
chmod +x source.sh
