(TeX-add-style-hook
 "report"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "12pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("geometry" "margin=1in")))
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperref")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperimage")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperbaseurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "nolinkurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "url")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "path")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "url")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "path")
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art12"
    "geometry"
    "amsmath"
    "amsthm"
    "amssymb"
    "enumerate"
    "graphicx"
    "hyperref"
    "xcolor"
    "epstopdf")
   (TeX-add-symbols
    "N"
    "Z")
   (LaTeX-add-environments
    '("corollary" LaTeX-env-args ["argument"] 1)
    '("question" LaTeX-env-args ["argument"] 1)
    '("problem" LaTeX-env-args ["argument"] 1)
    '("exercise" LaTeX-env-args ["argument"] 1)
    '("lemma" LaTeX-env-args ["argument"] 1)
    '("theorem" LaTeX-env-args ["argument"] 1))))

