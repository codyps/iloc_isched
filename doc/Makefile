TARGET := hw
TRASH  := $(TARGET).pdf \
          $(TARGET).dvi \
          $(TARGET).aux \
          $(TARGET).log \
	  $(TARGET).bbl \
          $(TARGET).blg \
	  $(TARGET).out

.PHONY: all clean
.SUFFIXES:
.SECONDARY:

all:: $(TARGET).pdf

%.ps : %.dot
	dot -Tps2 -o$@ $<

%.pdf : %.ps
	ps2pdf $< $@

clean :
	rm -f $(TRASH)

%.pdf: %.tex combined.tex
	pdflatex -interaction=nonstopmode $*
	pdflatex -interaction=nonstopmode $*
