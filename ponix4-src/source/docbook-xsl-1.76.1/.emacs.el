(add-hook
  'nxml-mode-hook
  (lambda ()
    (setq rng-schema-locating-files-default
          (append '("/source/docbook-xsl-1.76.1/locatingrules.xml")
                  rng-schema-locating-files-default ))))
