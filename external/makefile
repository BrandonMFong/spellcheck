##
# author: brando
# date: 11/15/24
#

### Dependencies

LIBS_BIN_PATH = bin/libs

build: build-libs

clean: clean-libs
	rm -rfv bin

## Libs

clean-libs:
	rm -rfv $(LIBS_BIN_PATH)

setup-libs:
	mkdir -p $(LIBS_BIN_PATH)

LIBS = bflibc bflibcpp bftest
LIBS_ASSETS = libs/bin/debug libs/bin/release
build-libs: setup-libs $(LIBS)
	cp -afv $(LIBS_ASSETS) $(LIBS_BIN_PATH)

$(LIBS):
	cd libs && make $@

