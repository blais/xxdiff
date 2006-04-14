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

(provide 'xxdiff)
