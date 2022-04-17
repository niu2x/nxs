all: all_nxs

all_nxs:
	$(MAKE) -C src/nxs nxsc

clean: 
	$(MAKE) -C src/nxs clean
