OPTIONS=--columns=80 --wrap=auto

all: html/index.html agaconv.1

html/index.html:
	@$(MAKE) -C ../src doc --silent
	@echo "Generated Doxygen html documentation at doc/html/index.html"

# agaconv.1 is distributed, in case agaconv is built on a system without
# pandoc. 
agaconv.1: agaconv.1.md
	@pandoc agaconv.1.md -s -t man -o agaconv.1 $(OPTIONS)
	@echo "Generated Linux man page at doc/agaconv.1"

clean:
	rm -rf html

