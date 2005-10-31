#!/bin/sh

if [ x$SVN_XXDIFF = x ]
then
	case "$0" in */*) cmd="$0";; *) cmd="./$0";; esac

	SVN_XXDIFF=1; export SVN_XXDIFF
	exec svn diff --diff-cmd "$cmd" "$@"
	exit 1
fi

fail () { echo $0: "$@" >&2; sleep 2; }

[ x"$7" = x ] && fail Required file argument"(s)" missing.
[ -f "$7"  ] || fail $7: Not a file.

echo
echo "***" Executing xxdiff --geometry 1200x600-0+0 "$6" "$7"
echo
echo "***" Press Ctrl-C in this window to break all following diffs.
echo
echo "***" Note, you can edit the file you are diffing and then rediff
echo "***" it with the original by pressing Ctrl-R on xxdiff window.
echo

exec xxdiff --geometry 1200x600-0+0 "$6" "$7"
