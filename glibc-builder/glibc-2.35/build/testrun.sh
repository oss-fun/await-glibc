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
    exec strace  -EGCONV_PATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/iconvdata  -ELOCPATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/localedata  -ELC_ALL=C  /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf/ld-linux-x86-64.so.2 --library-path /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/math:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/dlfcn:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nss:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nis:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/rt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/resolv:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/mathvec:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/support:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/crypt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nptl ${1+"$@"}
    ;;
  rpctrace)
    exec rpctrace  -EGCONV_PATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/iconvdata  -ELOCPATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/localedata  -ELC_ALL=C  /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf/ld-linux-x86-64.so.2 --library-path /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/math:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/dlfcn:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nss:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nis:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/rt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/resolv:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/mathvec:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/support:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/crypt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nptl ${1+"$@"}
    ;;
  valgrind)
    exec env GCONV_PATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/iconvdata LOCPATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/localedata LC_ALL=C valgrind  /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf/ld-linux-x86-64.so.2 --library-path /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/math:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/dlfcn:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nss:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nis:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/rt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/resolv:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/mathvec:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/support:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/crypt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nptl ${1+"$@"}
    ;;
  container)
    exec env GCONV_PATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/iconvdata LOCPATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/localedata LC_ALL=C  /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf/ld-linux-x86-64.so.2 --library-path /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/math:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/dlfcn:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nss:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nis:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/rt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/resolv:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/mathvec:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/support:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/crypt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nptl /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/support/test-container env GCONV_PATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/iconvdata LOCPATH=/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/localedata LC_ALL=C  /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf/ld-linux-x86-64.so.2 --library-path /home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/math:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/elf:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/dlfcn:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nss:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nis:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/rt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/resolv:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/mathvec:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/support:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/crypt:/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/build/nptl ${1+"$@"}
    ;;
  *)
    usage
    ;;
esac
