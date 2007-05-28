# This file is part of the xxdiff package.  See xxdiff for license and details.
#
# Note: this has originally been distributed by Michalis Giannakidis, under the
# name cvsxxdiff.py.  This script was integrated in april 2006 in the xxdiff
# scripts.

"""xx-cvs-revcmp [<options>] [FILE ...]

Display file differences with the current or a previous CVS version.  Use one or
two of the version selection options to select which filesets to compare.


Here is the original submission email description:

  What, here in our company, find useful about cvsxxdiff is:

  - cvsxxdiff is one file. No python module directory or initialization is
    required.

  - Do a 'cvs up; cvsxxdiff' before commit to see all the local changes we have
    made.

  - Do 'cvs up; cvsxxdiff -b 1 file.c' to see the last chages that were made on
    a file by a colleague.

  - We often need to track bugs in our code. This means going back to previous
    revisions, thus doing ex: 'cvsxxdiff -b 1 -b 2 file.c' to compare the
    previous with the 2nd previous revision. (similar with cvsxxdiff -r 1.1 -r
    1.2 file.c)

  - The utility prints  in the standard output the logs between revisions i
    and j ( ex: cvsxxdif -b i -b j file.c)

  One script to do it all.

"""

__author__ = ('Michalis Giannakidis <mgiannakidis@gmail.com>',
              'Martin Blais <blais@furius.ca>')


# stdlib imports.
import sys, os, re, tempfile
from subprocess import Popen, PIPE, call
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke
from xxdiff.scripts import tmpprefix


def find_in_trunk(search_directories, fn):
    """
    Recursively search for paths of all files whose basename matches 'fn' under
    the directories from 'search_directories' and return a list of them.
    """
    # Make sure that fn is always just a basename.
    assert os.pathsep not in fn

    # Run the search.
    files = []
    for d in search_directories:
        for root, dirs, files_in_dir in os.walk(d):
            if 'CVS' in dirs:
                dirs.remove('CVS')  # don't visit CVS directories
            if fn in files_in_dir:
                files.append(join(root, fn))
    return files

match_status = re.compile('File:\s*([^\s]+)\s*Status: (.*)')

def collect_unupdated_files(diff_directories, resolve_conflicts):
    """
    Collect all files that are not updated (compared to the repository).
    """
    assert diff_directories

    p = Popen(['cvs', 'status'] + diff_directories, stdout=PIPE)
    stdout, stderr = p.communicate()
    lines = stdout.splitlines()

    collected_files = []
    for l in lines:
        m = match_status.search(l)
        if not m:
            continue

        if resolve_conflicts:
            if m.group(2) != 'File had conflicts on merge':
                continue
        else:
            if m.group(2) == 'Up-to-date':
                continue
        fn = m.group(1)

        # Run the search from the CWD if there are no diff dirs.
        files = find_in_trunk(diff_directories, fn)
        if files:
            if len(files) > 1:
                print '%s more than once' % fn
                # FIXME: if we find a file more than once,
                # keep the unupdated.
            collected_files.extend(files)
        else:
            print '%s: No such file' % fn

    return collected_files

def get_local_trunk_version(filename):
    """
    For filename, get the current revision number (of the local version).
    """
    dn, bn = split(filename)

    lines = open(join(dn, 'CVS', 'Entries')).readlines()
    match_unupdated = re.compile('/%s/([0-9\.]*)' % bn)
    for l in lines:
        m = match_unupdated.search(l)
        if  m:
            return m.group(1)
    return '1.1'

match_repository_rev = re.compile('Repository revision:\s([0-9\.]*)')

def get_repository_revision(filename):
    """
    Get the Repository revision of a file.
    """
    p = Popen(['cvs', 'status', filename], stdout=PIPE)
    stdout, stderr = p.communicate()
    for l in stdout.splitlines():
            m = match_repository_rev.search(l)
            if m:
                return m.group(1)
    return '1.1'

def get_previous_revision(filename, back):
    """
    For filename, get its previous revision number (compared to the local
    version).
    """
    r = get_local_trunk_version(filename).split('.')
    r[-1] = str(int(r[-1]) - back)
    r = '.'.join(r)
    return r

def get_revisions_between(r1, r2):
    """
    Return a list with the CVS version strings between to releases.
    """
    rev = []

    l_r1 = r1.split('.')
    l_r2 = r2.split('.')
    try:
        beg = int(l_r1[-1])
        end = int(l_r2[-1])
    except ValueError:  
        return rev

    if end <= beg:
        rg = xrange(beg, end-1, -1)
    else:
        rg = xrange(beg, end+1)
    for r in rg:
        l_r1[-1] = str(r)
        rev.append('.'.join(l_r1))

    return rev

match_log_start = re.compile('^----------------------------')

def get_revision_log(filename, rev):
    """
    Get the CVS log of a filename given a revision.
    """
    log = []

    # Note: watch out for the 'cvs log' command, the value of the -r option must
    # not contain spaces before the value, it must be lumped together with the
    # option.  This is a CVS options parsing bug.
    p = Popen(['cvs', 'log', '-r%s' % rev, filename], stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    # Note: we're simply throwing away stderr here.

    start_log = False
    for l in stdout.splitlines():
        if start_log == 0:
            if match_log_start.search(l):
                start_log = True
                continue
        if start_log:
            log.append(l)

    return log


def mkheader(filename):
    """
    Build a header for display.
    """
    return ('>%s<' % filename).center(80).replace(' ', '_')


def cvsxxdiff_bi_bj(diff_files, prevcounts):
    """
    Compare two relative revision numbers.
    """
    for fn in diff_files:
        print mkheader(fn)

        # Get revision numbers.
        v1, v2 = [get_previous_revision(fn, prevcounts[x]) for x in 0, 1]

        # Get the files from the server.
        tmpfiles = []
        for v in v1, v2:
            f = tempfile.NamedTemporaryFile('w+', prefix=tmpprefix)
            tmpfiles.append(f)
            call(['cvs', 'update', '-r', v, '-p', fn], stdout=f)

        # Print the revisions.
        revs = get_revisions_between(v1, v2)
        for r in revs:
            print '\n'.join(get_revision_log(fn, r))

        # Launch xxdiff.
        xxdiff.invoke.xxdiff_display(
            opts,
            '--title1', "%s ( %s )" % (fn, v1),
            '--title2', "%s ( %s )" % (fn, v2),
            *[x.name for x in tmpfiles])

def cvsxxdiff_bi(diff_files, prevcount):
    """
    Compare to one relative revision number.
    """
    for fn in diff_files:
        print mkheader(fn)

        # Get revision numbers.
        v = get_previous_revision(fn, prevcount)
        vl = get_local_trunk_version(fn)

        # Print the revisions.
        revs = get_revisions_between(v, vl)
        for r in revs:
            print '\n'.join(get_revision_log(fn, r))

        # Get the other file from the server.
        p = Popen(['cvs', 'update', '-r', v, '-p', fn], stdout=PIPE)

        # Launch xxdiff.
        xxdiff.invoke.xxdiff_display(
            opts,
            '--title1', ('%s ( %s ) File as %d versions ago' %
                         (fn, v, prevcount)),
            '--title2', ('%s ( %s ) local version' %
                         (fn, vl)),
            '-', fn, stdin=p.stdout)

def cvsxxdiff_ri(diff_files, action):
    """
    Compare to one absolute revision number.
    """
    for fn in diff_files:
        print mkheader(fn)

        # Print the revisions.
        if action[0] == 'r':
            print '\n'.join(get_revision_log(fn, action[1]))

        # Launch xxdiff.
        p = Popen(['cvs', 'update', '-%s' % action[0], action[1], '-p', fn],
                  stdout=PIPE)

        xxdiff.invoke.xxdiff_display(
            opts,
            '--title1', '%s ( %s )'  % (fn, action[1]),
            '--title2', ('%s ( %s ) local version' %
                         (fn, get_local_trunk_version(fn))),
            '-', fn, stdin=p.stdout)

def cvsxxdiff_ri_rj(diff_files, actions):
    """
    Compare to two absolute revision numbers.
    """
    revisions = [x[1] for x in actions]
    for fn in diff_files:
        print mkheader(fn)

        # Fetch the temporary files.
        tmpfiles = []
        for code, rev in actions:
            f = tempfile.NamedTemporaryFile('w+', prefix=tmpprefix)
            tmpfiles.append(f)
            call(['cvs', 'update', '-%s' % code, rev, '-p', fn], stdout=f)

        # Print revisions.
        if actions[0][0] == 'r' and actions[1][0] == 'r':
            revs = get_revisions_between(revisions[0], revisions[1])
            for r in revs:
                print '\n'.join(get_revision_log(fn, r))

        # Launch xxdiff.
        xxdiff.invoke.xxdiff_display(
            opts,
            '--title1', "%s ( %s )" % (fn, revisions[0]),
            '--title2', "%s ( %s )" % (fn, revisions[1]),
            *[x.name for x in tmpfiles])

def cvsxxdiff_rep(diff_files):
    """
    Compare checkout to repository.
    """
    for fn in diff_files:
        print mkheader(fn)

        # Get the revisions.
        vr = get_repository_revision(fn)
        vl = get_local_trunk_version(fn)

        # Print the revisions.
        revs = get_revisions_between(vr, vl)
        for r in revs:
            print '\n'.join(get_revision_log(fn, r))

        # Launch xxdiff.
        p = Popen(['cvs', 'update', '-p', fn], stdout=PIPE)

        xxdiff.invoke.xxdiff_display(
            opts,
            '--title1', '%s ( %s ) cvs version' % (fn, vr),
            '--title2', '%s ( %s ) local version' % (fn, vl),
            '-', fn, stdin=p.stdout)

def cvsxxdiff_c(diff_files):
    """
    Resolve conflicts.
    """
    for fn in diff_files:
        print mkheader(fn)

        # Get revision numbers.
        vl = get_local_trunk_version(fn)
        print '\n'.join(get_revision_log(fn, vl))


        # Launch xxdiff.
        xxdiff.invoke.xxdiff_display(
            opts,
            '--resource', 'Show.PaneMergedView: True',
            '--unmerge',
            fn)

def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    group = optparse.OptionGroup(parser, "Options for selecting the revisions",
                                 "These options allow you to specify the "
                                 "revisions to compare.  You can specify zero, "
                                 "one or two of the following options.  The "
                                 "default is otherwise HEAD (latest).")

    group.add_option('-r', '--revision-tag', '--tag', action='append',
                     metavar='TAG', dest='revisions', default=[],
                     help="A tag like CVS.  See cvs(1).")

    group.add_option('-D', '--date-spec', action='append',
                     dest='dates', default=[],
                     help="A date spec like CVS.  See cvs(1).")

    group.add_option('-b', '--nb-revisions-before', action='append', type='int',
                     metavar='NBREVS', dest='before', default=[],
                     help="The number of previous CVS revisions from BASE.")

    parser.add_option_group(group)

    parser.add_option('-c', '--resolve-conflicts', action='store_true',
                      help="Resolve conflicts in merged files.")

    xxdiff.invoke.options_graft(parser)

    xxdiff.scripts.install_autocomplete(parser)
    opts, args = parser.parse_args()

    xxdiff.invoke.options_validate(opts, parser)

    # Figure out the two actions from the options that were passed in.  There
    # can be at most two actions.  Each action is specified as a pair of
    # (action-code, spec).
    actions = []
    for code, optlist in (('b', opts.before),
                          ('r', opts.revisions),
                          ('D', opts.dates)):
        actions.extend((code, x) for x in optlist)

    if len(actions) > 2:
        parser.error("You cannot specify more than two revision selection "
                     "options.")

    return opts, actions, args


def revcomp_main():
    """
    Command-line interface (looks at sys.argv to decide what to do).
    """
    global opts
    opts, actions, filenames = parse_options()

    # Separate files and directories in arguments.
    diff_files, diff_directories = [], []
    for fn in filenames:
        if isfile(fn):
            diff_files.append(fn)
        elif isdir(fn):
            diff_directories.append(fn)

    # Collect files.
    #
    # If no files were given on the command line or command was invoked on 
    # specific directories, search for unupdated files recursively in
    # o) in the specified directories or
    # o) the current dir
    if len(diff_files) == 0 or len(diff_directories) :
        collected_files = collect_unupdated_files(diff_directories or ['.'],
                                                  opts.resolve_conflicts)
        diff_files.extend(collected_files)

    if len(diff_files) > 1 :
        # Print a message to the user about the list of files to diff.
        # Print it in a single line. This allows to copy paste the output to
        # ex. a 'cvs commit' command.
        print '%d files to diff:' % len(diff_files)
        print '=' * 80
        print ' '.join(diff_files)
        print '=' * 80

    # Dispatch to the appropriate method.
    if opts.resolve_conflicts:
        # ex: -c
        if actions:
            raise SystemExit("You cannot specify revision selectors when "
                             "resolving conflicts.")
        cvsxxdiff_c(diff_files)

    # With a single revision selector.
    elif len(actions) == 1:
        code, count = actions[0]
        if code == 'b':
            # ex: -b1
            cvsxxdiff_bi(diff_files, count)

        elif code in ('r', 'D'):
            # ex: -r1.2  # or with date_spec
            cvsxxdiff_ri(diff_files, actions[0])

        else:
            raise SystemExit("Unsupported revision combination.")

    # With two revision selectors.
    elif len(actions) == 2:
        codes = tuple([x[0] for x in actions])
        revs = tuple([x[1] for x in actions])

        if codes == ('b', 'b'):
            # ex: -b1 -b2
            cvsxxdiff_bi_bj(diff_files, revs)

        if codes[0] in ('r', 'D') and codes[1] in ('r', 'D'):
            # ex: -r1.2 -r1.3
            cvsxxdiff_ri_rj(diff_files, actions)

        else:
            raise SystemExit("Unsupported revision combination.")

    elif not actions:
        # ex: cvsxxdiff
        cvsxxdiff_rep(diff_files)


def main():
    xxdiff.scripts.interruptible_main(revcomp_main)

if __name__ == '__main__':
    main()

