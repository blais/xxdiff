;; -*- mode: lisp-interaction; fill-column: 80 -*-
;;
;; xxdiff support for emacs.
;;
;; Author: Martin Blais <blais@furius.ca>
;;
;; This module provides various functions for invoking xxdiff from emacs.  See
;; the source code for details.

(require 'ediff-util)

(defvar xxdiff-exec "xxdiff"
  "The xxdiff executable to run.")

(defvar xxdiff-options '("-w" "-b")
  "A list of options to pass to xxdiff when called from emacs with a prefix argument.")

(defun xxdiff-diff-buffer-and-file ()
  "Runs xxdiff between the current buffer contents and the file on disk."
  (interactive)
  (let ((opts (if current-prefix-arg xxdiff-options nil)))
    (apply 'call-process-region
	   (append (list (point-min) (point-max) 
			 xxdiff-exec nil nil nil)
		   opts
		   (list "--title1" (format "%s (FILE)" (buffer-file-name))
			 "--title2" (format "%s (BUFFER)" (buffer-name))
			 (buffer-file-name) "-")))))

(defun xxdiff-svn-diff ()
  "Diffs the current buffer and the latest Subversion file.
FIXME TODO: Instead of doing this, we should customize vc.el to
make the diff program selectable as a dynamic variable and
override it in this function."
  ;; FIXME: change vc.el :: vc-diff-internal instead, see below.
  (interactive)

  (let ((merged-fn (concat "/tmp/xxdiff.merged." 
			   (prin1-to-string (emacs-pid)) "." (buffer-name)))
	(latest (concat (file-name-directory (buffer-file-name))
			".svn/text-base/"
			(file-name-nondirectory (buffer-file-name))
			".svn-base"))
	(opts (if current-prefix-arg xxdiff-options nil)))

    (apply 'call-process-region
	   (append (list (point-min) (point-max) 
			 xxdiff-exec nil nil nil)
		   opts
		   (list "--title1" (format "%s (FILE)" latest)
			 "--title2" (format "%s (BUFFER)" (buffer-name))
			 "--merged-filename" merged-fn
			 latest "-")))
    (when (file-exists-p merged-fn)
      (kill-region (point-min) (point-max))
      (insert-file merged-fn)
      (delete-file merged-fn)
      (message "Restored buffer contents from xxdiff merged output."))
    ))


(defun xxdiff-compare-kill-ring ()
  "Spawn an xxdiff to compare the last two entries on the
kill-ring.  With prefix arg, compare the last three."
  (interactive)

  (let* ((idx (if mark-active -1 0))
	 (selections (list (current-kill (+ idx 1) t)
			   (if (= idx 0) 
			       (current-kill idx t) 
			     (buffer-substring (region-beginning) (region-end)))))
	 files)
    (when current-prefix-arg
      (setq selections (append (list (current-kill (+ idx 2) t)) selections))
      ;;(setcdr selections (cons (current-kill 2 t) (cdr selections)))
      )

    (setq files (mapcar 
		 (lambda (sel) (let ((fn (make-temp-file "xxdiff-emacs.")))
				 (with-temp-file fn (insert sel))
				 fn))
		 selections))

      (apply 'call-process-region
	     (append (list (point-min) (point-max) 
			   xxdiff-exec nil nil nil)
		     files))
      (dolist (fn files)
	(when (file-exists-p fn) (delete-file fn)))
      ))


(defun xxdiff-compare-selections (beg end)
  "If the secondary selection is active, compare that to the
primary selection.  If not, compare the last entry in the
kill-ring to the primary selection (the active region).  If the
region is not active (we have no primary selection), use the two
first entries on the kill-ring.  This actually writes the regions
as temporary files and then invokes xxdiff."
  (interactive "r")
  (let (primary secondary)

    (if mark-active
	(setq primary 
	      (buffer-substring (region-beginning) (region-end))

	      secondary
	      (condition-case nil 
		  (prog1 (x-get-selection 'SECONDARY)
		    (message "xxdiff: Comparing against the secondary selection."))
		(error (progn
			 (message "xxdiff: Comparing against the last value on the kill-ring.")
			 (current-kill 0 t)))))

      (setq primary 
	    (current-kill 0 t)	      
	    secondary
	    (current-kill 1 t)))

    (let ((opts (if current-prefix-arg xxdiff-options nil))
	  (left (make-temp-file "xxdiff-emacs."))
	  (right (make-temp-file "xxdiff-emacs.")))

      (with-temp-file left (insert secondary))
      (with-temp-file right (insert primary))

      (apply 'call-process-region
	     (append (list (point-min) (point-max) 
			   xxdiff-exec nil nil nil)
		     opts
		     (list "--title1" "(SECONDARY REGION)"
			   "--title2" "(PRIMARY REGION)"
			   left right)))
      (when (file-exists-p left) (delete-file left))
      (when (file-exists-p right) (delete-file right))
      )))

(provide 'xxdiff)

