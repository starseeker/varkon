#####################################################################
#
#  This makefile preprocesses all htm-files in current directory
#  and move the resulting htm-files to corresponding directory 
#  under VARKON_ROOT/man
#
#  (c) 2007 Sören Larsson, Örebro University
#
#####################################################################

TARGETDIR = ../../../man

INCLUDEFILES = ../include/header.inc \
               ../include/footer.inc \
               ../include/svnversion.inc

HTMSOURCES = $(wildcard *.htm)
HTMRESULT  = $(patsubst %.htm, $(TARGETDIR)/%.htm, $(HTMSOURCES))

CSSOURCES  = $(wildcard *.css)
CSSRESULT  = $(patsubst %.css, $(TARGETDIR)/%.css, $(CSSOURCES))

JPGSOURCES = $(wildcard *.jpg)
JPGRESULT  = $(patsubst %.jpg, $(TARGETDIR)/%.jpg, $(JPGSOURCES))

PNGSOURCES = $(wildcard *.png)
PNGRESULT  = $(patsubst %.png, $(TARGETDIR)/%.png, $(PNGSOURCES))

GIFSOURCES = $(wildcard *.gif)
GIFRESULT  = $(patsubst %.gif, $(TARGETDIR)/%.gif, $(GIFSOURCES))

TXTSOURCES = $(wildcard *.txt)
TXTRESULT  = $(patsubst %.txt, $(TARGETDIR)/%.txt, $(TXTSOURCES))

all : $(HTMRESULT) $(CSSRESULT) $(JPGRESULT) $(PNGRESULT) $(GIFRESULT) $(TXTRESULT)
	@echo Creating subdirectories if needed
	mkdir -p ../../../man/tutorial
	mkdir -p ../../../man/GUI
	mkdir -p ../../../man/MBS
	mkdir -p ../../../man/MBS/images
	mkdir -p ../../../man/appdev
	mkdir -p ../../../man/sysadm
	mkdir -p ../../../man/relnotes
	@echo make contents in subdirectories
	$(MAKE) -C tutorial
	$(MAKE) -C GUI
	$(MAKE) -C MBS
	$(MAKE) -C MBS -C images
	$(MAKE) -C appdev
	$(MAKE) -C sysadm
	$(MAKE) -C relnotes

# preprocess htm-files, copy other files

$(TARGETDIR)/%.htm : ./%.htm $(INCLUDEFILES)
	m4 <$*.htm >$(TARGETDIR)/$*.htm

$(TARGETDIR)/%.css : ./%.css
	cp $*.css $(TARGETDIR)/$*.css

$(TARGETDIR)/%.jpg : ./%.jpg
	cp $*.jpg $(TARGETDIR)/$*.jpg

$(TARGETDIR)/%.png : ./%.png
	cp $*.png $(TARGETDIR)/$*.png

$(TARGETDIR)/%.gif : ./%.gif
	cp $*.gif $(TARGETDIR)/$*.gif

$(TARGETDIR)/%.txt : ./%.txt
	cp $*.txt $(TARGETDIR)/$*.txt

clean:
	rm -f -r $(TARGETDIR)

