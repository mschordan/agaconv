bin/agaconv: 
	$(MAKE) -C src --silent
	@mkdir -p bin
	@cp src/agaconv bin
	@echo "Generated executable at 'bin/agaconv'. It can be copied to any location."

all: bin/agaconv docs

docs:
	$(MAKE) -C doc --silent

view-man: docs doc/agaconv.1
	cat doc/agaconv.1 | man -l -

clean:
	$(MAKE) -C src clean
	$(MAKE) -C doc clean
	rm -fr bin
