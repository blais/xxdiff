#!/usr/bin/env python
"""
A quick script to extract the diffs of contributions from other people, to be
able to review them quickly.
"""

import re
from subprocess import *



def main():
    import optparse
    parser = optparse.OptionParser(__doc__.strip())
    opts, args = parser.parse_args()

    fn, = args
    revs = []
    for line in open(fn):
        mo = re.match('^changeset:[ \t]+([0-9]+):', line)
        if not mo:
            continue
        revs.append(int(mo.group(1)))

    files = set()
    for rev in revs:
        p = Popen('hg diff -r %s' % rev, stdout=PIPE, shell=1, text=True)
        out, _ = p.communicate()
        print()
        print()
        print(rev)
        print()
        print(out)
        ## mo = re.search('files:(.*)\n', out)
        ## files |= set(mo.group(1).split())
        
    print(files)
    for fn in sorted(files):
        print(fn)





if __name__ == '__main__':
    main()
