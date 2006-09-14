;; -*- mode: lisp-interaction; fill-column: 80 -*-
;;
;; xxdiff support for emacs.
;;
;; Author: Martin Blais <blais@furius.ca>
;;
;; This module provides various functions for invoking xxdiff from emacs.  See
;; the source code for details.

(require 'ediff-util)

(defun xxdiff-diff-buffer-and-file ()
  "Runs xxdiff between the current buffer contents and the file on disk."
  (interactive)
  (call-process-region (point-min) (point-max) 
		       "xxdiff" nil nil nil 
		       "--title1" (format "%s (FILE)" (buffer-file-name))
		       "--title2" (format "%s (BUFFER)" (buffer-name))
		       (buffer-file-name) "-"))

(defun xxdiff-svn-diff ()
  "Diffs the current buffer and the latest Subversion file.
FIXME TODO: Instead of doing this, we should customize vc.el to
make the diff program selectable as a dynamic variable and
override it in this function."
;; FIXME: change vc.el :: vc-diff-internal instead, see below.
  (interactive)
  (let ((latest (concat (file-name-directory (buffer-file-name))
			".svn/text-base/"
			(file-name-nondirectory (buffer-file-name))
			".svn-base")))
    (call-process-region (point-min) (point-max) 
			 "xxdiff" nil nil nil
			 "--title1" (format "%s (FILE)" latest)
			 "--title2" (format "%s (BUFFER)" (buffer-name))
			 latest "-")))


(provide 'xxdiff)
