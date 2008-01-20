# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions that contain stuff related to PostgreSQL.

In particular, functions to dump and parse a PostgreSQL database schema.  We use
this to perform schema comparison in one of the scripts.
"""

__author__ = "Martin Blais <blais@furius.ca>"


# stdlib imports.
import os, optparse, re
from os.path import *
from subprocess import Popen, PIPE

# xxdiff imports.
from xxdiff.utils import consepairs


def options_graft(parser):
    """
    Graft options on given parser for SQL connections.
    """
    group = optparse.OptionGroup(parser, "SQL connection options",
                                 "These options affect how we connect to "
                                 "SQL databases.")

    group.add_option('-H', '--host', action='store',
                     default=None,
                     help="Database server host or socket directory")

    group.add_option('-p', '--port', action='store',
                     default=None,
                     help="Database server port number")

    group.add_option('-U', '--username', '--user', action='store',
                     default=None,
                     help="Connect as specified database user")

    parser.add_option_group(group)

    return group


def options_validate(opts, parser, logs=None):
    """
    Validate replacement options.
    """
    args = []
    if opts.host:
        args.extend( ['-h', opts.host] )
    if opts.port:
        args.extend( ['-p', opts.port] )
    
    opts.pgsqlargs = args


dbspec_re = re.compile(
    '^(?:([a-zA-Z0-9_]+)@)?([a-zA-Z0-9_]+)(?:/([a-zA-Z0-9_]+))?$')

def parse_dbspec(dbspec, parser, opts):
    """
    Parse the DB spec and return the user, dbname, schema components.

    DB specs are of the form:  [user@]<dbname>[/schema]
    """
    mo = dbspec_re.match(dbspec)
    if not mo:
        parser.error("Invalid database/schema specification: '%s'" % dbspec)

    user, db, schema = mo.groups()

    # Set defaults from the options.
    if user is None:
        user = opts.username
        
    return Schema(dbspec, user, db, schema)


class Schema(object):
    """
    Container for all database schema-related things.
    """
    def __init__(self, dbspec, user, dbname, schema):
        self.dbspec = dbspec
        self.user, self.dbname, self.schema = user, dbname, schema
        self.dump = None
        
def dump_schema(user, dbname, schema, opts):
    """
    Dump the schema for the given database.  Returns the dump text on output.
    """
    tmplcmd = ['pg_dump', '--schema-only'] + opts.pgsqlargs

    cmd = list(tmplcmd)
    if user:
        cmd.extend(['-u', user])
    if schema:
        cmd.extend(['--schema', schema])
    cmd.append(dbname)

    p = Popen(cmd, stdout=PIPE)
    dump, stderr = p.communicate()
    if p.returncode != 0:
        raise SystemExit("Error: Dumping database '%s':\n%s\n" %
                         (dbname, stderr))

    return dump


sec_re = re.compile('^-- (?:Data for )?Name:\s*([^\s;]+);\s*Type:\s*([^;]+);(.*)$', re.M)
com_re = re.compile('--.*$', re.M)
ct_re = re.compile('^CREATE TABLE.*?(\\().*?(\\);)', re.M|re.S)

def parse_dump(dbdump, sort_columns=False):
    """
    Parse a PostgreSQL database dump, extracting all its section into a list of
    (name, type, contents) tuples. The entries are built from the Name and Type
    fields in the header comments. Contents of data chunks for tables are
    returned as type 'DATA', so if you're going to turn this into a map, be
    careful to filter out the 'data' entries or concatenate them to the schema
    (whichever is appropriate for your application).
    """
    # Class to contain info about chunks.
    class Chunk:
        def __init__(self, mo):
            self.mo = mo
            self.name, self.typ = mo.group(1, 2)

    # Parse chunks.
    chunks = map(lambda mo: Chunk(mo), sec_re.finditer(dbdump))
    for c1, c2 in consepairs(chunks):
        c1.contents = dbdump[c1.mo.end():c2.mo.start()]
    chunks[-1].contents = dbdump[chunks[-1].mo.end():]

    # Remove comments in the contents.
    for c in chunks:
        c.descline = c.mo.group(0)
        c.contents = com_re.sub('', c.contents).strip() + '\n'
        c.mo = None # release the match objects

        # Sort columns in CREATE TABLE statements.
        if sort_columns:
            mo = ct_re.match(c.contents)
            if mo:
                pre, post = c.contents[:mo.end(1)], c.contents[mo.start(2):]
                columns = c.contents[mo.end(1):mo.start(2)].strip()
                line_cols = map(lambda x: x.endswith(',') and x or '%s,' % x,
                                map(str.strip, columns.splitlines()))
                line_cols.sort()
                c.contents = (pre + '\n' +
                              ''.join('   %s\n' % x for x in line_cols) +
                              post)

    return list((c.name, c.typ, c.descline + '\n\n' + c.contents)
                for c in chunks)

def dump_chunks(chunks):
    """
    Debug printout of chunks.
    """
    for chunk in chunks:
        print '========', chunk.name, chunk.typ
        print chunk.descline
        print chunk.contents

