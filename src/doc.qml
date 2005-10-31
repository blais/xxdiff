<h1>xxdiff documentation</h1>

<p>
This is the user's manual for xxdiff, version <b><version></b>.
The official location of xxdiff on the internet is
<a href="http://xxdiff.sourceforge.net">http://xxdiff.sourceforge.net</a>.
If you're reading this page from the web, note that you can also access this
document from the Help menu of xxdiff.
</p>

<!-- ----------------------------------------------------------------------- -->
<h2>Table of Contents</h2>

<ul>
<li><a href="#introduction">Introduction</a></li>
<li><a href="#invocation">Invocation</a></li>
<li><a href="#general">General operation</a></li>
<li><a href="#compfiles2">Comparing two files</a></li>
<li><a href="#compfiles3">Comparing three files</a></li>
<li><a href="#compdirs2">Comparing two directories</a></li>
<li><a href="#merging">Merging files and resolving conflicts</a></li>
<li><a href="#review">Merge review features</a></li>
<li><a href="#editing">Editing files</a></li>
<li><a href="#customization">Customization using resource file</a></li>
<li><a href="#resources">Resource reference</a></li>
<li><a href="#faq">Frequently Asked Questions (FAQ)</a></li>
<li><a href="#author">Author</a></li>
<li><a href="#bugs">Reporting bugs</a></li>
<li><a href="#history">Project history</a></li>
</ul>

<!-- ----------------------------------------------------------------------- -->
<a name="introduction">
<h2>Introduction</h2>

<p>
xxdiff is a <i>graphical file and directories comparison and merge
tool</i>. This program is an essential software development tool that can be
used to
<ul>
<li> visualize the differences between files or directories;
<li> merge files with differences, resolving conflicts and saving output to a
new file or patch;
<li> assist file changes reviewing and comment production (e.g. approving source
code changes before they get merged into a source tree).
</ul>
</p>

<p>
xxdiff doesn't itself compute the differences between files, it just displays
them graphically (the rationale behind this is that you can use different tools
to compute the actual diffs (for example using Rational ClearCase's cleardiff,
which gives more sensible results than GNU diff in certain cases), 
there is a relatively standard format for difference output (POSIX diff),
so it is easy and efficient to parse diff output from different programs,
under the UNIX dataflow paradigm, it is common for graphical tools to be
built on top of text tools. There are already many good tools to perform diffs,
so there's no need to rewrite them.
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="invocation">
<h2>Invocation</h2>

<p>
Usage:
<blockquote><tt>
xxdiff [OPTIONS] &lt;file1&gt; &lt;file2&gt; [&lt;file3&gt;]
</tt></blockquote>
</p>

<p>
You can specify either two files (two-way diff), three files (three-way diff),
or two directories (directory diff).
</p>

<p>
You can also specify a mix of regular files and directories.  If at least one of
the paths specified on the cmdline is a regular file, all other paths that are
directories will have the basename of the regular filename appended to them.
For example, if you specify:
<blockquote><tt>
xxdiff dir1/bn1 dir2/bn2 dir3
</tt></blockquote>

xxdiff will be run on <tt>dir1/bn1</tt>, <tt>dir2/bn2</tt>, and
<tt>dir3/bn1</tt>.
</p>

<p>
xxdiff can accept one of its inputs from stdin. You must replace the relevant
filename by '<tt>-</tt>' (a dash) to instruct xxdiff to use stdin for that file,
e.g.
<blockquote><tt>
cat file2 | sort | xxdiff file1 - file3
</tt></blockquote>
</p>

<p>
If all the file paths are directories, a directory diff is invoked.  Note that
xxdiff doesn't do three-way directory diffs.
</p>

<!-- ---------------------------------------- -->
<a name="options">
<h3>Options</h3>

<invocation>

<p>
Note that the last few options are provided compatible with common GNU diff
options that the user might want to use, as a mere convenience.  Note also that
xxdiff options are a superset of the original xdiff options, so that xxdiff is
compatible with old SGI scripts that use xdiff.  xxdiff options follow the GNU
conventions for command-line options.
</p>


<!-- ---------------------------------------- -->
<h3>Using xxdiff as a short-lived application</h3>
    
<p>
xxdiff has not been designed with the intent of being used over multiple sets of
files over the course of one execution---that is, it has been built around the
idea that you would execute it once over a single set of inputs, use it, then
exit, rather than to open new files again using the `file' menu features without
exiting.  The `file' menu features are provided as a convenience, to allow one
to occasionally replace one of the files during an execution.  That is the
reason why you can't open xxdiff with just one file.  It is just not useful that
way.
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="general">
<h2>General operation</h2>

<p>
This section describes general features and operation of xxdiff.
</p>

<!-- ---------------------------------------- -->
<h3>The text area</h3>

<p>
xxdiff shows up with two or three text regions that display the text of the
files to compare, with color-coded blocks that indicate regions of text (lines)
that have changes (specific sections contain a list and descriptions on what
types of change can appear).  Each of these regions of text is called a "hunk"
in diff terminology (for more about this, see the GNU diff manual).  The text
area can be scrolled vertically and horizontally, when necessary.
</p>

<p>
Corresponding regions of text are aligned between the files.  Since changed
regions do not necessarily contains the same number of lines, empty alignment
lines are appended to the shorter regions in order to keep the corresponding
lines aligned (this is a simple way of solving the alignment problem; an
alternate solution would have been to implement a smart type of scrolling with
lines indicating correspondance of regions between them). 
</p>

<p>
All the colors can be individually configured using the display options dialog
or resources.  The display dialog contains a legend of all the colors.  Note
that the default colors for different regions are often set to be the same to
other types of lines when it is not necessary to dissociate them, to avoid
visual clutter.  You could however, customize every region to be of a different
color, for example, perhaps you would want to have the dummy lines drawn of a
special color to indicate that they are not really part of the text.
</p>

<p>
In the text area, there is a cursor that in indicates the current line of text
being worked on.  This cursor can be optionally hidden.
<p>


<h4>Horizontal diffs</h4>

<p>
Each of the diff hunks is normally displayed in one color, but if horizontal
diffs is enabled, for each line, xxdiff find the smallest differing part of the
line that differs and displays that in a different, highlighted color.  This
allows the user to more quickly visualize exactly what has changed between the
two lines of text.  No need to mention, this is highly addictive and is now
enabled by default.
</p>

<p>
When computing horizontal diffs, whitespace can be optionally ignored.
</p>

<h4>Navigation</h4>

<p>
xxdiff supports a subset of the emacs bindings, as well as support for most of
the standard keys found on AT keyboards (PgUp, PgDown, arrow keys, Home, End).
</p>

<p>
Default bindings include:
<ul>
<li>Ctrl-V, Alt-V: scroll up/down;</li>
<li>Ctrl-N, Ctrl-P: move cursor one line up/down;</li>
<li>Home, End: move cursor to beginning/end of files;</li>
<li>N, P: move to next/previous diff hunk;</li>
<li>B, O: mov the next/previous <b>unselected</b> diff hunk.</li>
</ul>
Explore the menus to find out about the other default bindings.  You can
customize all the key bindings using the resources.
</p>


<h4>Pop-up menu</h4>

<p>
Using the third mouse button in the text area brings a popup menu that contains
cursor-sensitive commands, as well as the common commands to navigate between
hunks.
</p>

<h4>Searching</h4>

<p>
You can search the text by bringing up the search dialog.  Type your search
query and press enter, which will apply the search and display diamond shaped
match locations in the overview area. You can then navigate the search results
using Ctrl-F and Ctrl-B.  Searching will occur in all files simultaneously.  You
have to reapply the search to search for something else.
</p>

<p>
You can also move the cursor and display to a particular line from the search
dialog.
</p>

<h4>Other options</h4>

<p>
Carriage returns (CR) characters in DOS files can be hidden in the text area.
</p>

<p>
Tabs by default are rendered aligned at each 8 characters, but can be set to any
value by changing the tab width option.
</p>

<h4>Display options</h4>

<p>
The display options can be fully customized by bringing up the options dialog
with the "Display -&gt; Display Options..." menu item.  From that dialog you can
also customize the colors and fonts.
</p>

<h3>Redoing the diffs</h3>
      
<p>
If a file has changed while xxdiff is up, and there is need to recompute the
diffs, invoke the "redo diff" command from the edit menu.  The arguments used
with the surrogate diff command can be changed by using the dialog provided by
the "diff arguments" command.  One can also replace a loaded file with a
different one by using the "open" commands from the "file" menu.
</p>


<!-- ---------------------------------------- -->
<h3>Other UI elements</h3>

<p>
The following other UI elements can be optionally hidden or displayed from the
Windows menu or Display menu (use the resource file to display or hide them on
startup):
<ul>

<li>
<b>Overview area.</b> On the right side, an area that shows a color-coded
pictorial rendering of the diff hunks for the whole files, with correspondances.
This area also shows the as a rectangle the currently visible portion of text
for each of the files.  This rectangle can also be used as a scrollbar.  In
addition, a marker indicates the location of the line cursor.
</li>

<li>
<b>Toolbar.</b> A toolbar that gives access to the common motion and selection
commands.
</li>

<li>
<b>Filenames.</b> For each file, a region at the top that displays the filename.
You can customize what shows up in this region from the command-line.  Clicking
on the filename puts it in the clipboard for cut-n-paste.  (This label will
intelligently display the end part of the filename if it is too long to fit.)
This region also display the text line that the line cursor is in, for each
file.  You can customize what gets displayed in this labeled by using a
command-line option.
</li>

<li>
<b>Line numbers.</b> For each file, a thin window that display the number of
each line (dummy alignment lines don't have any number).
</li>

<li>
<b>Vertical line.</b> A vertical line can be drawn in the text area, at a
specified column.  This can be used for alignment purposes or for verifying that
a line doesn't extend beyond a specified number of characters (many people like
to limit their source code to fit in e.g. 80 characters).
</li>

<li>
<b>Merged view</b>. A view of the file resulting from the currently selected
regions (see section on <a href="#mergedview">merged view</a> below).
</li>

</ul>
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="compfiles2">
<h2>Comparing two files</h2>

<p>
When invoking xxdiff with two files, we normally think of an old and a new file,
where
<blockquote><tt>
xxdiff old new
</tt></blockquote>

The default colors used here are symmetrical, so invoking the files in a
different order will result in regions being assigned the same colors.
</p>

<h3>Types of hunks</h3>

<p>
The following hunk types can appear when comparing two files:
<ul>
<li> <b>Same</b>: no change in that region;</li>
<li> <b>Insert</b>: text has been added to the new file;</li>
<li> <b>Delete</b>: text has been deleted from the new file;</li>
<li> <b>Diff</b>: text has changed between the files.</li>
</ul>
Note that each Diff or Insert hunk is always separated by a Same hunk (if it was
not the case, then two different contiguous hunks would necessarily get merged
into a single Diff hunk).
</p>

<h3>Options</h3>

<p>
You can ignore trailing blanks, whitespace, or case for two-way diffs.  See GNU
diff(1) manual for information these features.  xxdiff provides menu items to
access these diff options. Those items simply add or remove switches to the diff
program command-line and call the diff program again to regenerate the diffs.
</p>

<p>
Switches for GNU diff algorithm quality are also provided in the UI as a
convenience.
</p>


<!-- ----------------------------------------------------------------------- -->
<a name="compfiles3">
<h2>Comparing three files</h2>

<p>
When invoking xxdiff with three files, we think of an ancestor file, a "mine"
file, which the current user has been working on, and a "yours" file which
someone else has merged in the main tree.  They are normally specified in the
following order:
<blockquote><tt>
xxdiff mine older yours
</tt></blockquote>

The default colors used here are symmetrical, so invoking the files in a
different order will result in regions being assigned the same colors.
</p>

<h3>Types of hunks</h3>

<p>
The following hunk types can appear when comparing two files:
<ul>
<li> <b>Same</b>: no change in that region;</li>
<li> <b>Insert</b>: text has been added one of the files;</li>
<li> <b>Delete</b>: text has been deleted from one of the files;</li>
<li> <b>DiffOne</b>: text is different in one of the three files but the two
other files are the same.</li>
<li> <b>DiffAll</b>: text is different in all of the three files.</li>
<li> <b>DiffDel</b>: text is different in two files and is absent in the
third.</li>
</ul>
</p>

<h3>Ignoring one file</h3>

<p>
In three-way mode, it can be useful to temporarily ignore one of the three files
to concentrate on the differences between the other two.  This feature is
available from the Display menu and is performed by simply remapping colors at
display time and doesn't require xxdiff to recompute the differences.  The
ignored file will be displayed as grayed out.
</p>


<!-- ----------------------------------------------------------------------- -->
<a name="compdirs2">
<h2>Comparing two directories</h2>

<p>
When invoking xxdiff with two directories, we normally think of an old and a new
directory, where
<blockquote><tt>
xxdiff olddir newdir
</tt></blockquote>

The default colors used here are symmetrical, so invoking the files in a
different order will result in regions being assigned the same colors.
</p>

<h3>Types of hunks</h3>

<p>
The following hunk types can appear when comparing two directories:
<ul>
<li> <b>Same</b>: files are present in both directories and have no
differences;</li>
<li> <b>Insert</b>: file has been added to directory;</li>
<li> <b>Delete</b>: file has been removed from directory;</li>
<li> <b>Diff</b>: files are present in both directories and have
differences;</li>
<li> <b>Directories</b>: directories are present in both directories but we
don't know if they differ;</li>
</ul>
</p>

<h3>Options</h3>

<p>
The directories can be compared recursively, in which case no directories will
be shown. You can activate this from the command-line or from the ui.
</p>

<p>
You can also instruct xxdiff not to display the files that haved changed, just
the insertions and deletions.
</p>

<h3>File operations</h3>

<p>
In directory diffs mode, you can invoke an xxdiff on the pair of files or
directories under the line cursor.  This allows you to start xxdiff on a pair of
directories, and then to examine the pairs of files that have changed, all from
that directory diff.  That functionality is available from the popup menu in the
text area (right-click).
</p>

<p>
You can also copy and remove files from that menu, see
<a href="#merging">merging</a> section for details.
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="merging">
<h2>Merging files and resolving conflicts</h2>

<p>
Merging files is the process of manually resolving conflicts when two files
cannot be merged automatically.  This is a common problem when doing parallel
software development where multiple developers could be working on the same
files.
</p>

<p>
Most of the merging can be handled automatically, because if the changes were
applied to fairly independent parts of the files, most of the changes will be
insertions or deletions.  However, when overlapping changes occur, no automatic
decision can be taken.
</p>

<p>
xxdiff can be used to resolve conflicts by letting the user "select" which side
of a diff hunk should be killed and what side should be kept.  The user can take
this decision on a line-by-line basis, and can decide to keep both sides as
well, in any order.  xxdiff then allows the user to save the resulting file.
</p>

<!-- ---------------------------------------- -->
<h3>Selecting regions</h3>

<p>
Selecting a diff hunk can be carried out by clicking on it.  The colors update
to display which side is selected for output. You can select individual lines
with the second button.  There are three types of selection that you can carry
out:
<ul>
<li>select clicked file;</li>
<li>select no file (take the decision that nothing will be output);</li>
<li>leave the line/hunk unselected.</li>
</ul>
</p>

<p>
If you want to select more than one side, you have to invoke the split-swap-join
command (default is on 'S' key), which will split the current diff hunk if it's
a Diff hunk so you can select the pieces you want from both sides.  Further
invocations of this command will cause swapping the regions, looping through all
the different ordering possibilities, and finally joining the regions again
(preserving selections where it is possible).
</p>

<!-- ---------------------------------------- -->
<h3>Saving the resulting file</h3>

<p>
The resulting file can be save under one of the input filenames, or under a new
filename (from the File menu).  If some regions are left unselected, the save
dialog will have some widgets that allows you to decide how you want to save the
unselected hunks:
<ul>
<li><b>Save as merge conflicts:</b> save all sides, separating them with merge
conflict markers similar to those inserted by CVS;</li>

<li><b>Save with conditionals:</b> save all sides, separating them with C
pre-processor directives, so that you could then use a #define directive to have
one set of changes or another be active if compiling.</li>
</ul>
The markers and pre-processor directives can be customized through the resources
file.
</p>

<p>
There is a customization flag that lets you specify that xxdiff should warn you
when trying to save with some regions unselected.  It will bring the cursor to
the first remaining unselected region and ask for confirmation. This can be used
to insure that the user has made all the required decisions before saving and
that no text will be forgotten.
</p>

<p>
You could also decide to save only the selected regions, from the File menu.
This can be useful for making comments when reviewing merges.
</p>

<!-- ---------------------------------------- -->
<h3>Rapid, sequential merge conflict resolution</h3>

<p>
Some people like to merge files within a guided process, where the visual diff
program asks for a merge decision for each conflictual hunk one-by-one and then
asks for a filename to use for saving the merged output.  For example, that is
how ClearCase's xcleardiff works when querying the user to resolve conflicts.
</p>

<p>
There are a set of commands under the Region menu that allows one to rapidly
take decisions on merge conflicts.  These commands perform a selection and then
automatically move the cursor to the next unselected region.  If there are no
more unselected regions, the save dialog is brought up automatically.
</p>

<!-- ---------------------------------------- -->
<h3>Automatic merge</h3>

<p>
By default, xxdiff doesn't perform any selections.  You can instruct xxdiff to
perform selection of the non-conflictual regions automatically.  This can be
specified from the command-line or from the Global menu.
</p>

<!-- ---------------------------------------- -->
<h3>Merging directories</h3>

<p>
Selecting is disable in directory diffs mode, where the concept of "saving" a
directory does not really make sense.  However, you can apply changes to a
directory from xxdiff, such as copying and removing files under the line cursor.
This can be invoked from the popup menu in the text area.
</p>


<!-- ---------------------------------------- -->
<a name="mergedview">
<h3>Merged view</h3>

<p>
During selection or before saving the selected regions into the result file, you
might want to visualize what it would look like with the current selections.
You can display the "merged view" for this purpose, an dedicated window
showing the merge output, that updates interactively as you make selections.
</p>

<p>
This view will scroll to match the main display.  Unselected regions are shown
in this view with oblique lines ("in construction" style), and merge decisions
are slightly highlighted as well.  This output allows you to review the merge
decisions before commiting the file to output.
</p>


<!-- ----------------------------------------------------------------------- -->
<a name="review">
<h2>Merge review features</h2>

<p>
One of the most important uses of xxdiff is to assist the merge review process,
where one developer visualizes the file changes of another and approves or
rejects those changes, making comments along the way.
</p>

<p>
For this purpose, xxdiff puts selected text in the clipboard text buffer when
you select it.  Thus, the merge cop can select a region, and paste the results
in a neighboring editor window.
</p>

<p>
These snippets of code will typically be interspersed with comments.  xxdiff can
be format the line contents before they are copied to the clipboard, to make
them distinct from the merge comments.  This can be enabled using the "format
clipboard text" option.  The clipboard format is a string that contains format
specifiers similar to printf.  The following specifiers are allowed:
<ul>
<li><b>%N</b>: file line (0 is leftmost file);</li>
<li><b>%L</b>: line number;</li>
<li><b>%F</b>: filename;</li>
<li><b>%s</b>: line contents.</li>
</ul>
</p>

<p>
Also note that clicking on the filename labels will put the filenames in the
clipboard for similar cut-n-paste action.  This can be useful when reviewing
ClearCase revision files, which often have very long p-names.
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="editing">
<h2>Editing files</h2>

<p>
There has been a conscious decision NOT to develop xxdiff into a fully-featured
editing program.  Sometimes it could be handy to be able to edit a few lines of
a difficult conflict.  However, that is a task best left to your own favorite
editing program.  Saving with merge conflicts will allow you to resolve those
few remaining conflicts by hand.
</p>
    
<p>
However, a few commands have been provided under the File menu to spawn an
editor on the input files.  You can then redo the diff after your input file has
changed.
</p>


<!-- ----------------------------------------------------------------------- -->
<a name="customization">
<h2>Customization using resource file</h2>

<p>
Much of the state of xxdiff can be customized through a resource file (actually,
some of the features can only be changed through resource customization).
By default, xxdiff looks for the file <tt>.xxdiffrc</tt> located in the user's
home directory (this file location can be overriden by setting the environment
variable XXDIFFRC), and reads that file for resource definition.
</p>

<p>
The format of the resource file is a fairly simple list of colon-separated
resource name/value pairs, one per line:
<blockquote><tt>
resourceName: "value"
</tt></blockquote>
Any text after "#" on a line is considered a comment and ignored.
</p>

<p>
Some resources may require string delimiters (double-quotes) around their value
specification, but not all (see below).
</p>

<p>
You can also specify additional resources on the command-line, which take
precendence over the resources specified in the resource file.  To get a
complete list of the available resources, use the <tt>--list-resources</tt>
command-line option (see <a href="#options">options section</a>).
</p>

<p>
Trick: you can use different resources files by invoking xxdiff like this in
most shells:
<blockquote><tt>
XXDIFFRC=myrcfile xxdiff file1 file2
</tt></blockquote>
</p>

<!-- ---------------------------------------- -->
<h3>Automatic generation of the resource file</h3>

<p>
You can generate a resource file containing only the differences from the
defaults from the "Help -&gt; Generate init file" menu command.  This can be
used to "save" the current customizable options.
</p>


<!-- ----------------------------------------------------------------------- -->
<a name="resources">
<h2>Resource reference</h2>

<p>
This section contains a reference of the all the currently supported
customizable resources in this version of xxdiff.
</p>

<resourceref>

<!-- ----------------------------------------------------------------------- -->
<a name="faq">
<h2>Frequently Asked Questions (FAQ)</h2>

<!-- ---------------------------------------- -->
<h3>Can I diff files with CRLF DOS characters in them?</h3>

<p>
If you want to diff files with CRLF characters in them, you can use the 'ignore
whitespace option to GNU diff, which will work, even if only one of the files
has the DOS characters in it.
</p>

<!-- ---------------------------------------- -->
<h3>How do I make xxdiff display the revision number of a file?</h3>

<p>
By default, the filenames window display the filename.  From the command-line,
howver, you can set it to display an arbitrary string, which could include your
current revision number.
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="author">
<h2>Author</h2>

<p>
This program has been written from scratch and is being actively maintained by
<b>Martin Blais</b>
(<a href="mailto:blais@iro.umontreal.ca">blais@iro.umontreal.ca</a>).
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="bugs">
<h2>Reporting bugs</h2>
      
<p>
All bug reports and feature requests should be submitted throught the
SourceForge bug tracking system at the following URL
<a href="http://sourceforge.net/bugs/?group_id=2198">
http://sourceforge.net/bugs/?group_id=2198</a>.  Please avoid sending the author
email about xxdiff bugs.  Please use this bug tracking system, it is extremely
fast and simple to use.
</p>

<!-- ----------------------------------------------------------------------- -->
<a name="history">
<h2>Project history</h2>

<p>
This program has been written and inspired after gdiff, mgdiff, and Rudy
Wortel's xdiff under SGI. xxdiff aims to provide a lasting open-source
replacement to these programs.  Writing this tool from scratch was originally
motivated by the need to have horizontal diffs, which turned out to be an
essential feature of this program.
</p>

<p>
This program is being extensively tested by several software development
engineers at discreet, that use it daily in their merge reviewing activities.
It has reached a stable state.
</p>

<h3>Licensing</h3>

<p>
This program is distributed under the GNU General Public License. See the
<a href="http://www.gnu.org">GNU project website</a> for full details.
</p>

<h3>Dependencies</h3>

This program depends on the availability of a textual diff program (such as GNU
diff) and the Qt UI toolkit (available at http://trolltech.com).

<h3>Acknowledgements</h3>

<p>
Thanks go to discreet, for letting me deploy my program there, and for not
bothering me with the occasional bug fix that I did on the job.  Many more
thanks to all the many engineers who tested it there and reported problems with
it, as well as providing invaluable suggestions.
</p>

<p>
SourceForge is hosting my web site and I'm using many of its features.  Tomas
Pospisek (tpo@sourcepole.ch) has provided a Debian package.  Dan Weeks has
packaged it for one of the BSD variants (danimal@danimal.org).
</p>

