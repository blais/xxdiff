# -*- mode: fundamental -*-
#Distribution: xxdiff
product xxdiff
	id "A graphical front end to the diff command"

	image man
		id "Man Pages"
        	version 1
		order 9999
		subsys legal
			id "GPL Legal Notice"
			default
			exp xxdiff.man.legal
		endsubsys
		subsys relnotes
			id "Release Notes"
			default
			exp xxdiff.man.relnotes
		endsubsys
		subsys manpages
			id "Man Pages"
			default
			exp xxdiff.man.manpages
		endsubsys
		subsys test
			id "Testing files"
			exp xxdiff.man.test
		endsubsys
	endimage

	image sw
		id "Software"
		version 3002
		order 9999
		subsys base
			id "Base Software"
			default
			exp xxdiff.sw.base
			prereq ( 
				fw_qt3.sw.lib 1230000000 1239999900 
				eoe.sw.base 1274627340 1274627340
			)
		endsubsys
	endimage
endproduct
