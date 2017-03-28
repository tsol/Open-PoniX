# Library for simple test scripts
# Copyright (C) 2009 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

require_cat() {
    if ! type cat > /dev/null 2> /dev/null; then
	echo "This test requires the cat utility" >&2
	exit 2
    fi
}

require_diff() {
    case "`diff --version 2> /dev/null`" in
    *GNU*)
	;;
    *)
	echo "This test requires GNU diff" >&2
	exit 2
    esac
}

have_ed() {
    type ed >/dev/null 2>/dev/null
}

use_tmpdir() {
    tmpdir=`mktemp -d`
    if test -z "$tmpdir" ; then
	echo "This test requires the mktemp utility" >&2
	exit 2
    fi
    cd "$tmpdir"
}

use_local_patch() {
    test -n "$PATCH" || PATCH=$PWD/src/patch

    eval 'patch() {
	if test -n "$GDB" ; then
	  echo -e "\n" >&3
	  gdbserver localhost:53153 $PATCH "$@" 2>&3
	else
          $PATCH "$@"
	fi
    }'
}

clean_env() {
    unset PATCH_GET POSIXLY_CORRECT QUOTING_STYLE SIMPLE_BACKUP_SUFFIX \
	  VERSION_CONTROL PATCH_VERSION_CONTROL GDB
}

_check() {
    _start_test "$@"
    expected=`cat`
    if got=`set +x; eval "$*" 3>&2 </dev/null 2>&1` && \
            test "$expected" = "$got" ; then
	echo "ok"
	checks_succeeded="$checks_succeeded + 1"
    else
	echo "FAILED"
	if test "$expected" != "$got" ; then
	    echo "$expected" > expected~
	    echo "$got" > got~
	    diff -u -L expected -L got expected~ got~
	    rm -f expected~ got~
	fi
	checks_failed="$checks_failed + 1"
    fi
}

check() {
    _check "$@"
}

ncheck() {
    _check "$@" < /dev/null
}

cleanup() {
    checks_succeeded=`expr $checks_succeeded`
    checks_failed=`expr $checks_failed`
    checks_total=`expr $checks_succeeded + $checks_failed`
    status=0
    if test $checks_total -gt 0 ; then
	if test $checks_failed -gt 0 ; then
	    status=1
	fi
	echo "$checks_total tests ($checks_succeeded passed," \
	     "$checks_failed failed)"
    fi
    if test -n "$tmpdir" ; then
	set -e
	cd /
	chmod -R u+rwx "$tmpdir"
	rm -rf "$tmpdir"
    fi
    exit $status
}

if test -z "`echo -n`"; then
    if eval 'test -n "${BASH_LINENO[0]}" 2>/dev/null'; then
	eval '
	    _start_test() {
		echo -n "[${BASH_LINENO[2]}] $* -- "
	    }'
    else
	eval '
	    _start_test() {
		echo -n "* $* -- "
	    }'
    fi
else
    eval '
	_start_test() {
	    echo "* $*"
	}'
fi

require_cat
clean_env

checks_succeeded=0
checks_failed=0
trap cleanup 0

