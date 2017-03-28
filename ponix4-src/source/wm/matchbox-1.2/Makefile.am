SUBDIRS = src data 

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-snap`date +"%Y%m%d"`

upload: dist
	cvs2cl --hide-filenames; \
	scp README  handhelds.org:~/public_html/downloadables/matchbox/matchbox-readme.txt; \
	scp ChangeLog handhelds.org:~/public_html/downloadables/matchbox/matchbox-changelog.txt; \
        scp matchbox-@VERSION@.tar.gz handhelds.org:~/public_html/downloadables/matchbox/
	make -C ./doc manual.html
	scp -r doc/manual doc/img doc/screenshots handhelds.org:~/public_html/matchbox/
	scp -r doc/home.html handhelds.org:~/public_html/matchbox/index.html

