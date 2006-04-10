#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-sql-schemas [<options>] <db1> <db2>

Where <dbX> is of the form:   [user@]<dbname>[/schema]

Script that dumps the schemas for two PostgreSQL databases, re-orders the
contents so that they diff nicely, and spawns xxdiff on the results.

Note: for now this script only supports the PostgreSQL database.  If you want to
add support for another database, just add an option and an appropriate
xxdiff.db.* module rather than writing a dedicated script.  (I'm very flexible
about adding new features to this script.)
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'PostgreSQL']


# stdlib imports.
import os, optparse, re
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke
import xxdiff.mapcompare
from xxdiff.db import postgresql


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    xxdiff.mapcompare.options_graft(parser)
    postgresql.options_graft(parser)
    xxdiff.invoke.options_graft(parser)

    opts, args = parser.parse_args()

    xxdiff.mapcompare.options_validate(opts, parser)
    postgresql.options_validate(opts, parser)
    xxdiff.invoke.options_validate(opts, parser)
    
    if len(args) != 2:
        parser.error("You need to specify two databases/schemas to compare.")
    
    dbspec1, dbspec2 = args

    return (opts,
            postgresql.parse_dbspec(dbspec1, parser, opts),
            postgresql.parse_dbspec(dbspec2, parser, opts))

#-------------------------------------------------------------------------------
#
def sqlcompare_main():
    """
    Main program for schema comparison script.
    """
    opts, db1, db2 = parse_options()
    
    # Dump the database schemas.
    for db in db1, db2:
        dump = postgresql.dump_schema(db.user, db.dbname, db.schema, opts)

        # Parse the dumps, produce an adequate map of its contained objects.
        db.objmap = postgresql.parse_dump(dump)

    # List all objects that were found.
    for key in sorted(set(db1.objmap.keys() + db2.objmap.keys())):
        print key

    # Render comparable files.
    files = xxdiff.mapcompare.render_diffable_maps(opts, db1.objmap, db2.objmap)

    # Set displayed titles.
    titles = []
    for i, db in enumerate( (db1, db2) ):
        titles.extend(['--title%s' % (i+1), '%s (SCHEMA)' % db.dbspec])
        
    # Invoke xxdiff .
    xxargs = titles + [x.name for x in files]
    xxdiff.invoke.xxdiff_display(opts, *xxargs)


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(sqlcompare_main)

if __name__ == '__main__':
    main()

