MV = @mv
RM = @rm -f

.PHONY: all clean install uninstall

all: dsemu

dsemu:
	@$(MAKE) -C daemon/ dsemu
	$(MV) daemon/dsemu ./

clean:
	@$(MAKE) -C daemon/ clean
	$(RM) dsemu

install: dsemu
	@$(MAKE) -C scripts-install/ install

uninstall:
	@$(MAKE) -C scripts-install/ uninstall
