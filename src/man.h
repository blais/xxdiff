char manText[] = 
"\n"
"\n"
"\n"
"XXDIFF(1.6)					      XXDIFF(1.6)\n"
"\n"
"\n"
"NAME\n"
"       xxdiff - a graphical file comparator and merge tool\n"
"\n"
"SYNOPSIS\n"
"       xxdiff [OPTIONS] <path> <path> [<path>]\n"
"\n"
"OVERVIEW\n"
"       xxdiff is a graphical browser for viewing the differences\n"
"       between two files and can be used to produce a merged ver�\n"
"       sion.  The text of the two or three files are presented\n"
"       side by side with their differences highlighted for easy\n"
"       identification.\n"
"\n"
"       The paths supplied on the command line identify the files\n"
"       to be compared.	The paths may be one of three things:  a\n"
"       regular file, a directory or a '-', in which case the\n"
"       input for that path will be read from standard input.  If\n"
"       the paths are all directories, a directory diff will be\n"
"       done (unsupported at this moment).  If at least one path\n"
"       is a file or a '-', a file diff will be performed.  If\n"
"       there are directory names along with file names, the base�\n"
"       name of the file name will be appended to the directory\n"
"       names to find the other files to compare with.  If there\n"
"       are multiple file names specified and some directories\n"
"       names as well, xxdiff bails out, since it doesn't know\n"
"       which basename to use for the missing one.\n"
"\n"
"OPTIONS FOR XXDIFF\n"
"       --help  Show this help message.\n"
"\n"
"       --version, -v\n"
"	       Show the program version and compilation options\n"
"	       it was built with.\n"
"\n"
"       --list-resources\n"
"	       Output a list of resources with documentation and\n"
"	       default values.\n"
"\n"
"       -N <string>\n"
"	       When input is read from stdin this option speci�\n"
"	       fies the string to use in place of it's name in\n"
"	       the title bar.  If not specified the string\n"
"	       ``<stdin>'' is used.\n"
"\n"
"       -D      If there are no differences then exit quietly with\n"
"	       exit code of 0.\n"
"\n"
"       -w      Passed on to diff(1). Ignore whitespace when com�\n"
"	       paring the two files.\n"
"\n"
"       -b      Passed on to diff(1). Ignore trailing blanks when\n"
"	       comparing the two files.\n"
"\n"
"       -i      Passed on to diff(1). Ignore case when comparing\n"
"	       the two files.\n"
"\n"
"       -m      (3-way only)  Pre-select hunks similarly to the\n"
"	       diff3 -m feature (conflicts will be left unse�\n"
"	       lected).	 This feature assumes that the files are\n"
"	       given in the MINE OLDER YOURS order.\n"
"\n"
"DESCRIPTION\n"
"       The xxdiff window is divided into 3 main areas. These are,\n"
"       the menu bar at the top of the window, an overview area on\n"
"       the right side of the window and a text area occupying the\n"
"       remainder. The text area is further divided into two text\n"
"       windows, one for each file, a vertical slider between them\n"
"       and a horizontal slider underneath them.\n"
"\n"
"       The text in each of the text panes is divided into blocks\n"
"       of lines which are color-coded with one of five different\n"
"       colors.	This indicates that the block either differs\n"
"       between the two files, is identical between the two files,\n"
"       has been inserted in one file or the other, is for display\n"
"       purposes (to match an inserted block in the other file),\n"
"       or has been selected by the user (for writing to a merged\n"
"       file).\n"
"\n"
"       These colors can be modified by the user via the\n"
"       resources.  A legend display of the colors and their mean�\n"
"       ings is available from the Help menu.  Resources can be\n"
"       specified thru pairs of values in the format: <resource\n"
"       name>: <value> in a ~/.xxdiffrc file (the environment\n"
"       variable XXDIFFRC can override this location).\n"
"\n"
"       On the right-hand side of the display is an overview area;\n"
"       this shows the files in their entirety.\n"
"\n"
"DIAGNOSTICS\n"
"       Returns the exit status generated by the diff command.\n"
"       This is usually 0 for no differences, 1 for some differ�\n"
"       ences and 2 for errors.\n"
"\n"
"SEE ALSO\n"
"       diff(1), X(1)\n"
"\n"
"COPYRIGHT\n"
"       Copyright (C) 1999, 2000	 Martin Blais\n"
"\n"
"       See the GNU General Public License for a full statement of\n"
"       rights and permissions.\n"
"\n"
"AUTHOR\n"
"       Martin Blais <blais@iro.umontreal.ca>\n"
"\n"
"ACKNOWLEDGEMENTS\n"
"       To Rudy Wortel <rudy@alias.com> for writing xdiff,\n"
"       to Daniel Williams <dan@sass.com> for writing mgdiff,\n"
"       to Andrew C. Myers for writing gdiff.\n"
"\n"
"\n"
"\n"
"								1\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
;
