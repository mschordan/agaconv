all:
	make -C src
	cp src/agaconv-encode bin

docs:
	make -C src docs

clean:
	make -C src clean
	rm -f bin/agaconv-encode

install: all
	@echo "Executables are available in 'bin'. Copy to any location."
