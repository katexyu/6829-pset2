.PHONY: all
all: pdf

.PHONY: pdf
pdf: report.pdf

IMAGES := \
	a.png \
	pid-grid.png \
	pd-grid.png \
	pid-vs-pd-grid.png \
	pd-95th-percentile.png \

report.pdf: report.md $(IMAGES)
	pandoc --from=markdown --output=$@ $<

pid-vs-pd-grid.png: pid-vs-pd-grid.gnuplot pid-grid.dat pd-grid.dat
	gnuplot $<

%.png: %.gnuplot %.dat
	gnuplot $<

.PHONY: clean
clean:
	rm -rf *.pdf *.png
