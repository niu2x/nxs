all: nxs

nxs:
	$(MAKE) -C src/nxs nxs

clean: 
	$(MAKE) -C src/nxs clean
