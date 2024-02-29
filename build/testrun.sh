#!/bin/bash
builddir=`dirname "$0"`
GCONV_PATH="${builddir}/iconvdata"

usage () {
cat << EOF
Usage: $0 [OPTIONS] <program> [ARGUMENTS...]

  --tool=TOOL  Run with the specified TOOL. It can be strace, rpctrace,
               valgrind or container. The container will run within
               support/test-container.
EOF

  exit 1
}

toolname=default
while test $# -gt 0 ; do
  case "$1" in
    --tool=*)
      toolname="${1:7}"
      shift
      ;;
    --*)
      usage
      ;;
    *)
      break
      ;;
  esac
done

if test $# -eq 0 ; then
  usage
fi

case "$toolname" in
  default)
    exec   env GCONV_PATH="${builddir}"/iconvdata LOCPATH="${builddir}"/localedata LC_ALL=C  "${builddir}"/elf/ld-linux-x86-64.so.2 --library-path "${builddir}":"${builddir}"/math:"${builddir}"/elf:"${builddir}"/dlfcn:"${builddir}"/nss:"${builddir}"/nis:"${builddir}"/rt:"${builddir}"/resolv:"${builddir}"/mathvec:"${builddir}"/support:"${builddir}"/crypt:"${builddir}"/nptl ${1+"$@"}
    ;;
  strace)
    exec strace  -EGCONV_PATH=/home/mouse/await-glibc/build/iconvdata  -ELOCPATH=/home/mouse/await-glibc/build/localedata  -ELC_ALL=C  /home/mouse/await-glibc/build/elf/ld-linux-x86-64.so.2 --library-path /home/mouse/await-glibc/build:/home/mouse/await-glibc/build/math:/home/mouse/await-glibc/build/elf:/home/mouse/await-glibc/build/dlfcn:/home/mouse/await-glibc/build/nss:/home/mouse/await-glibc/build/nis:/home/mouse/await-glibc/build/rt:/home/mouse/await-glibc/build/resolv:/home/mouse/await-glibc/build/mathvec:/home/mouse/await-glibc/build/support:/home/mouse/await-glibc/build/crypt:/home/mouse/await-glibc/build/nptl ${1+"$@"}
    ;;
  rpctrace)
    exec rpctrace  -EGCONV_PATH=/home/mouse/await-glibc/build/iconvdata  -ELOCPATH=/home/mouse/await-glibc/build/localedata  -ELC_ALL=C  /home/mouse/await-glibc/build/elf/ld-linux-x86-64.so.2 --library-path /home/mouse/await-glibc/build:/home/mouse/await-glibc/build/math:/home/mouse/await-glibc/build/elf:/home/mouse/await-glibc/build/dlfcn:/home/mouse/await-glibc/build/nss:/home/mouse/await-glibc/build/nis:/home/mouse/await-glibc/build/rt:/home/mouse/await-glibc/build/resolv:/home/mouse/await-glibc/build/mathvec:/home/mouse/await-glibc/build/support:/home/mouse/await-glibc/build/crypt:/home/mouse/await-glibc/build/nptl ${1+"$@"}
    ;;
  valgrind)
    exec env GCONV_PATH=/home/mouse/await-glibc/build/iconvdata LOCPATH=/home/mouse/await-glibc/build/localedata LC_ALL=C valgrind  /home/mouse/await-glibc/build/elf/ld-linux-x86-64.so.2 --library-path /home/mouse/await-glibc/build:/home/mouse/await-glibc/build/math:/home/mouse/await-glibc/build/elf:/home/mouse/await-glibc/build/dlfcn:/home/mouse/await-glibc/build/nss:/home/mouse/await-glibc/build/nis:/home/mouse/await-glibc/build/rt:/home/mouse/await-glibc/build/resolv:/home/mouse/await-glibc/build/mathvec:/home/mouse/await-glibc/build/support:/home/mouse/await-glibc/build/crypt:/home/mouse/await-glibc/build/nptl ${1+"$@"}
    ;;
  container)
    exec env GCONV_PATH=/home/mouse/await-glibc/build/iconvdata LOCPATH=/home/mouse/await-glibc/build/localedata LC_ALL=C  /home/mouse/await-glibc/build/elf/ld-linux-x86-64.so.2 --library-path /home/mouse/await-glibc/build:/home/mouse/await-glibc/build/math:/home/mouse/await-glibc/build/elf:/home/mouse/await-glibc/build/dlfcn:/home/mouse/await-glibc/build/nss:/home/mouse/await-glibc/build/nis:/home/mouse/await-glibc/build/rt:/home/mouse/await-glibc/build/resolv:/home/mouse/await-glibc/build/mathvec:/home/mouse/await-glibc/build/support:/home/mouse/await-glibc/build/crypt:/home/mouse/await-glibc/build/nptl /home/mouse/await-glibc/build/support/test-container env GCONV_PATH=/home/mouse/await-glibc/build/iconvdata LOCPATH=/home/mouse/await-glibc/build/localedata LC_ALL=C  /home/mouse/await-glibc/build/elf/ld-linux-x86-64.so.2 --library-path /home/mouse/await-glibc/build:/home/mouse/await-glibc/build/math:/home/mouse/await-glibc/build/elf:/home/mouse/await-glibc/build/dlfcn:/home/mouse/await-glibc/build/nss:/home/mouse/await-glibc/build/nis:/home/mouse/await-glibc/build/rt:/home/mouse/await-glibc/build/resolv:/home/mouse/await-glibc/build/mathvec:/home/mouse/await-glibc/build/support:/home/mouse/await-glibc/build/crypt:/home/mouse/await-glibc/build/nptl ${1+"$@"}
    ;;
  *)
    usage
    ;;
esac
