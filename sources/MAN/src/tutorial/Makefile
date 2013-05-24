#####################################################################
#
#  This makefile preprocesses all htm-files in the tutorial directory
#  and moves the resulting htm-files to VARKON_ROOT/man/tutorial
#
#####################################################################

TARGETDIR = ../../../../man/tutorial

INCLUDEFILES = ../../include/header.inc \
               ../../include/footer.inc \
               ../../include/svnversion.inc

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

all : $(HTMRESULT) $(CSSRESULT) $(JPGRESULT) $(PNGRESULT) $(GIFRESULT)

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