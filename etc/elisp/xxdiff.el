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

;;------------------------------------------------------------------------------
;; For versioncontrol.

;; Do this by hand some other time.  You need to fetch the BASE and call
;; process-file.  Using vc is not going to help much, because this will only run
;; 'svn diff' in the background.

;; (defun vc-diff-xxdiff ()
;;   "Dispatches between vc-diff and vc-xxdiff on the prefix arg.
;; This is used to replace the other behaviour of the vc-diff
;; binding from the vc package."
;;   (interactive)
;;   (if current-prefix-arg
;;       (vc-xxdiff)
;;     (vc-diff nil)))

;; (defun vc-xxdiff ()
;;   "Runs xxdiff on the current buffer and the previous version.
;; This is the analogous to vc-diff."
;;   (interactive)
;;   ;; Hijack vc-diff-internal.
;;   (flet ((vc-diff-internal
;; 	  (file rev1 rev2) 
;; 	  ;; Cut-and-pasted from vc.el (2006-04-14) because it does not provide a
;; 	  ;; variable for running the diff program.  This really sucks, I should submit
;; 	  ;; a patch.
;; 	  (if (or (not rev1) (string-equal rev1 ""))
;; 	      (setq rev1 (vc-workfile-version file)))
;; 	  (if (string-equal rev2 "")
;; 	      (setq rev2 nil))
;; 	  (let ((file-rev1 (vc-version-backup-file file rev1))
;; 		(file-rev2 (if (not rev2)
;; 			       file
;; 			     (vc-version-backup-file file rev2)))
;; 		(coding-system-for-read (vc-coding-system-for-diff file)))
;; 	    (if (and file-rev1 file-rev2)
;; 		;; Here we replaced "diff" by "xxdiff".  That's it.
;; 		(apply 'vc-do-command "*vc-diff*" 1 "xxdiff" nil
;; ;; 		       (append (vc-switches nil 'diff)
;; ;; 			       ;; Provide explicit labels like RCS or CVS would do
;; ;; 			       ;; so diff-mode refers to `file' rather than to
;; ;; 			       ;; `file-rev1' when trying to find/apply/undo hunks.
;; ;; 			       (list "-L" (vc-diff-label file file-rev1 rev1)
;; ;; 				     "-L" (vc-diff-label file file-rev2 rev2)
;; 				     (file-relative-name file-rev1)
;; 				     (file-relative-name file-rev2))
;; ;; 	      ))
;; 		(progn
;; 		  (insert diff)

;; 	      (vc-call diff file rev1 rev2))))))
;;     (vc-diff nil)))

;; FIXME: remove when we figure this out.
;;(setq debug-on-error t)
;; (defun vc-diff-internal-for-xxdiff (file rev1 rev2)


(provide 'xxdiff)
