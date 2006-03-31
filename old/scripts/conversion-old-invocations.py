#-------------------------------------------------------------------------------
# findgrepsed:

        # Create a temporary file to contain the merged results.
        tmpf2 = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

        # Call xxdiff on it.
        status, xxdiff_output = commands.getstatusoutput(
            ('xxdiff --decision --merged-filename "%s" '
             '--title2 "NEW FILE" "%s" "%s" ') %
            (tmpf2.name, fn, tmpf.name))

        print xxdiff_output

        # Check out the exit code from xxdiff
        l1 = xxdiff_output.splitlines()[0]   # first line

        if l1 == 'ACCEPT':


#-------------------------------------------------------------------------------
# condreplace:

        tmpf2 = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

            cmd = ('%s --decision --merged-filename "%s" ' + \
                   '--title2 "NEW FILE" "%s" "%s" ') % \
                   (opts.xxdiff_exec, tmpf2.name, origfile, modfile)
            s, o = commands.getstatusoutput(cmd)


        if o == 'ACCEPT' ...


#-------------------------------------------------------------------------------
# cvsdiff:

            tmpf2 = NamedTemporaryFile('w', prefix=tmpprefix)

            cmd = ('xxdiff --decision --merged-filename "%s" ' +
                   '--title2 "NEW FILE" "%s" "%s" ') % \
                   (tmpf2.name, tmpf.name, filename)
            s, o = commands.getstatusoutput(cmd)

            # print output of xxdiff command.
            if o:
                print o


#-------------------------------------------------------------------------------
# encrypted:

    diffcmd = '%(xxdiff)s --merged-filename "%(merged)s" ' + \
              '--indicate-input-processed ' # + filenames

    fmerge = NamedTemporaryFile(prefix=tmpprefix)
    print '== TEMPFILE', fmerge.name

    m = {'xxdiff': opts.xxdiff_exec,
         'gpg': opts.gpg,
         'merged': fmerge.name}

    cmd = diffcmd % m + ' '.join(map(lambda x: '"%s"' % x.name, tempfiles))
    fout = os.popen(cmd, 'r')

    # Leave time for xxdiff to read the files before hiding, it will let us know
    # when it is done with the input files.
    l = fout.readline()
    assert l.startswith('INPUT-PROCESSED')

    fout.read()


#-------------------------------------------------------------------------------
# match:

        print ' '.join(l)
        cmd = ' '.join( ['xxdiff'] + extra + opts + l )
        os.popen(cmd)


#-------------------------------------------------------------------------------
# patch:

        dopts += '--title%d="%s (patched)"' % (pno, sfilename)
        dopts += opts.xxdiff_options
        cmd = 'xxdiff %s "%s" "%s"' % (dopts, leftfn, rightfn)

        s, o = commands.getstatusoutput(cmd)

        # print output of xxdiff command.
        if o: print o

        # perform the requested action
        if o and not opts.dry_run:

            if o == 'ACCEPT':


#-------------------------------------------------------------------------------
# diffproxy:

    tmpf = NamedTemporaryFile('rw', prefix=tmpprefix)

    # run command
    xxdiff_path = 'xxdiff'

    decision = len(args) == 3
    cmd = [xxdiff_path]
    if decision:
        cmd.append('--decision')
    if opts.merge:
        cmd.append('--merge')
    cmd.append('--merged-filename="%s"' % tmpf.name)
    for o1, o2, fn in zip(['--title1', '--title2', '--title3'],
                          ['"%s"' % x for x in opts.label],
                          args):
        if os.path.basename(fn) == 'empty-file':
            o2 = '"(NON-EXISTANT FILE)"'
        cmd.extend([o1, o2])
    cmd.extend(args)
    ##print ' '.join(cmd)

    s, o = commands.getstatusoutput(' '.join(cmd))

    if decision:
        # if the user merged, copy the merged file over the original.
        if o in ['MERGED', 'ACCEPT', 'REJECT']:

