#    Never export any environment variables, unless exported explicitly.
#    This keeps the environment small and surprises low.
#    Make sure that utilities have no hidden dependency on locales.
#    For example, date(1) output depends on LANG.
unexport                  # Never export to sub-makes.
unexport C_INCLUDE_PATH   # Remove from shell environment; confuses gcc.
export LANG   = C.UTF-8
export LC_ALL = C.UTF-8

.DEFAULT_GOAL = all

#   My helper binaries.
#
TOOLS = lscp hextobdf hextosrc srctohex txttopng

#   And their corresponding C language source files.
#
TOOLS_C = $(addsuffix .c,$(TOOLS))

#   And the preprocessed source for tag generation, etc.
#
PREPROCESSED = $(addsuffix .i,$(TOOLS))

#   What to build by default.
#
TOP_LEVEL_TARGETS = gallant.bdf
TOP_LEVEL_TARGETS += gallant.fnt
TOP_LEVEL_TARGETS += gallant.pcf.gz
TOP_LEVEL_TARGETS += gallant.ttf
TOP_LEVEL_TARGETS += 12x22.fnt.gz
TOP_LEVEL_TARGETS += images
TOP_LEVEL_TARGETS += README.html

.PHONY: all
all: $(TOP_LEVEL_TARGETS)

gallant.bdf: gallant.hex hextobdf
	./hextobdf < $< > $@

gallant.hex: gallant.src srctohex
	./srctohex < $< > $@

gallant.fnt: gallant.hex
	vtfontcvt -v -o $@ $^

gallant.pcf: gallant.bdf
	bdftopcf -o $@ $^

gallant.pcf.gz: gallant.pcf
	gzip -cnv9 $^ > $@

gallant.src: hextosrc
	./hextosrc < gallant.hex > $@

gallant.ttf: gallant.bdf
	@printf '%s\n' \
	'Open("$^")' \
	'SetFontNames("Gallant12", "Gallant12", "Gallant12")' \
	'Generate("$@", "ttf")' \
	'Quit()' | \
	SOURCE_DATE_EPOCH=$(TIMESTAMP) fontforge -lang=ff -script -
	fontlint $@

# make 12x22.fnt.gz: build the font the FreeBSD loader can use.
#
12x22.fnt.gz: gallant.fnt
	gzip -cnv9 $^ > $@

# make install-maintainer: install files on maintainer's system.
#
.PHONY: install-maintainer
install-maintainer: gallant.pcf.gz gallant.ttf gallant.fnt gallant.hex 12x22.fnt.gz
	cp gallant.pcf.gz gallant.ttf ~/.fonts
	cd ~/.fonts && mkfontdir && xset fp rehash && fc-cache
	if test $$(uname -s) = FreeBSD; then \
	  vidcontrol -f gallant.fnt < /dev/ttyv0; \
	  sudo cp gallant.fnt /usr/share/vt/fonts/gallant.fnt; \
	  sudo cp gallant.hex /home/toor/FreeBSD/head/src/share/vt/fonts/gallant.hex; \
	  sudo cp 12x22.fnt.gz /boot/fonts/12x22.fnt.gz; \
	fi

# make images: create the PNG files in Images/ for each block
#
.PHONY: images
images: gallant.hex lscp txttopng
	printf '%s\n' \
	'0020  0080 Basic-Latin' \
	'00A0  0100 Latin-1-Supplement' \
	'0100  0180 Latin-Extended-A' \
	'0180  0250 Latin-Extended-B' \
	'0250  02B0 IPA-Extensions' \
	'02B0  0300 Spacing-Modifier-Letters' \
	'0300  0370 Combining-Diacritical-Marks' \
	'0370  0400 Greek-and-Coptic' \
	'0400  0500 Cyrillic' \
	'0500  0530 Cyrillic Supplement' \
	'1E00  1F00 Latin-Extended-Additional' \
	'1F00  2000 Greek-Extended' \
	'2000  2070 General-Punctuation' \
	'2070  20A0 Superscripts-and-Subscripts' \
	'20A0  20D0 Currency-Symbols' \
	'20D0  2100 Combining-Diacritical-Marks-for-Symbols' \
	'2100  2150 Letterlike-Symbols' \
	'2150  2190 Number-Forms' \
	'2190  2200 Arrows' \
	'2200  2300 Mathematical-Operators' \
	'2300  2400 Miscellaneous-Technical' \
	'2400  2440 Control-Pictures' \
	'2440  2460 Optical-Character-Recognition' \
	'2460  2500 Enclosed-Alphanumerics' \
	'2500  2580 Box-Drawing' \
	'2580  25A0 Block-Elements' \
	'25A0  2600 Geometric-Shapes' \
	'2600  2700 Miscellaneous-Symbols' \
	'2700  27C0 Dingbats' \
	'27C0  27F0 Miscellaneous-Mathematical-Symbols-A' \
	'27F0  2800 Supplemental-Arrows-A' \
	'2800  2900 Braille-Patterns' \
	'2900  2980 Supplemental-Arrows-B' \
	'2A00  2B00 Supplemental-Mathematical-Operators' \
	'2B00  2C00 Miscellaneous-Symbols-and-Arrows' \
	'30A0  3100 Katakana' \
	'E0A0  E0F0 Private-Use-Area' \
	'FB00  FB50 Alphabetic-Presentation-Forms' \
	'FFF0 10000 Specials' | \
	while read -r first last name; do \
	  ./lscp "0x$$first" "0x$$last" > "$$name.txt"; \
	  ./txttopng -f "$<" -t "$$name.txt" -p "Images/$$first-$$name.png"; \
	  ./txttopng -f "$<" -t "$$name.txt" -p "Images/$$first-$$name-Inverted.png" -i; \
	done
	./txttopng -f "$<" -t sample.txt -p Images/sample.png -i

# make README.html: turn markdown into HTML.
#
README.html: README.md
	comrak --gfm --syntax-highlighting base16-ocean.light $^ > $@

VERSION = 2025-08-31
# Deterministic time stamp. On FreeBSD: date -j 202508310000.01 '+%s'
TIMESTAMP = 1756591201

# FreeBSD: Libs and <uniname.h> are in devel/libunistring
CC = cc -std=c99
APP_WARNS  += -Werror
APP_WARNS  += -Wall
APP_WARNS  += -Wextra
APP_WARNS  += -Wcast-align
APP_WARNS  += -Wcast-qual
APP_WARNS  += -Wfloat-equal
APP_WARNS  += -Wformat=2
APP_WARNS  += -Wno-format-nonliteral
APP_WARNS  += -Winline
APP_WARNS  += -Wstrict-prototypes
APP_WARNS  += -Wmissing-prototypes
APP_WARNS  += -Wunused
APP_WARNS  += -Wold-style-definition
APP_WARNS  += -Wpedantic
APP_WARNS  += -Wpointer-arith
APP_WARNS  += -Wredundant-decls
APP_WARNS  += -Wshadow
APP_WARNS  += -Wsign-conversion
APP_WARNS  += -Wswitch-enum
APP_WARNS  += -Wuninitialized
APP_WARNS  += -Wvla
APP_WARNS  += -Wwrite-strings
APP_WARNS  += -Wignored-qualifiers
APP_WARNS  += -Wmissing-field-initializers

APP_SOURCE_INCDIRS = -I /usr/local/include
APP_LIBDIRS = -L /usr/local/lib
APP_MACROS += -DVERSION='"$(VERSION)"'

################################################################################
#                           ____        _                                      #
#                          |  _ \ _   _| | ___  ___                            #
#                          | |_) | | | | |/ _ \/ __|                           #
#                          |  _ <| |_| | |  __/\__ \                           #
#                          |_| \_\\__,_|_|\___||___/                           #
#                                                                              #
#                        How to build stuff in general.                        #
################################################################################

#   Delete the match anything rules.
MAKEFLAGS += --no-builtin-rules

#   Clear the suffix list so no default rules are used.
.SUFFIXES:
.SUFFIXES: .c .i .o

#   How to compile a C source to an object file.
#
%.o: %.c
	$(CC) -c $(APP_CFLAGS) $(APP_WARNS) $(APP_SOURCE_INCDIRS) $(APP_MACROS) -o $@ $<

#   How to pre-process a C source file with the compiler and save result in a *.i file.
#
%.i: %.c
	$(CC) -E $(APP_CFLAGS) $(APP_WARNS) $(APP_SOURCE_INCDIRS) $(APP_MACROS) -o $@ $<


lscp: lscp.o
	$(CC) -o $@ $(APP_LIBDIRS) -luninameslist -lunistring $^

hextobdf: hextobdf.o
	$(CC) -o $@ $^

hextosrc: hextosrc.o
	$(CC) -o $@ $(APP_LIBDIRS) -luninameslist -lunistring $^

srctohex: srctohex.o
	$(CC) -o $@ $^

txttopng: txttopng.o
	$(CC) -o $@ $(APP_LIBDIRS) -lpng $^

################################################################################
#        _   _      _                   _____                    _             #
#       | | | | ___| |_ __   ___ _ __  |_   _|_ _ _ __ __ _  ___| |_ ___       #
#       | |_| |/ _ \ | '_ \ / _ \ '__|   | |/ _` | '__/ _` |/ _ \ __/ __|      #
#       |  _  |  __/ | |_) |  __/ |      | | (_| | | | (_| |  __/ |_\__ \      #
#       |_| |_|\___|_| .__/ \___|_|      |_|\__,_|_|  \__, |\___|\__|___/      #
#                    |_|                              |___/                    #
#                                                                              #
################################################################################

# make tools: just compile the TOOLS.
#
.PHONY: tools
tools: $(TOOLS)

#------------------------------------------------------------------------------#
#                                    Clean                                     #
#------------------------------------------------------------------------------#

# make clean: remove what can be regenerated.
#
.PHONY: clean
clean:
	rm -f *.i *.o *.gz $(TOOLS)
	rm -f gallant.bdf gallant.fnt gallant.hex gallant.pcf gallant.ttf

#------------------------------------------------------------------------------#
#                                     Lint                                     #
#------------------------------------------------------------------------------#

# make lint: run lint for each of the tool source files.
#
.PHONY: lint
lint: $(TOOLS_C)
	@for c in $^; do flexelint lint.lnt $$c; done

#------------------------------------------------------------------------------#
#                          Tags - Create tags for vi                           #
#------------------------------------------------------------------------------#

# Create tags from the actual files compiled, and the actually included
# headers. Examine the preprocessor output's '# LINE "FILE"' directives.

# make tags: create vi tags file.
#
CTAGS = jexctags
.PHONY: tags
tags: $(PREPROCESSED)
	@awk '/^# / {print $$3}' $^ | sort -u | grep -v \< | tr -d \" > list
	@$(CTAGS) -L list -f $@
	@$(CTAGS) -L list -f $@.p --language-force=c --c-kinds=p
	@rm -f list

# make typenames: output list of C language typedef names used in source code.
#                 Useful for .indent.pro.
#
.PHONY: typenames
typenames: $(PREPROCESSED)
	@awk '/^# / {print $$3}' $^ | sort -u | grep -v \< | tr -d \" | \
	$(CTAGS) -L - -f - --language-force=c --c-kinds=t | \
	cut -f 1 | sort -u

APP_SYSTEM_INCDIRS = -I/usr/include -I/usr/local/include
# make tooltips: create tooltips.vim for vim to source and show macros.
#
.PHONY: tooltips tooltips.vim
tooltips: tooltips.vim
tooltips.vim: $(TOOLS_C)
	{ \
	printf 'function! MyBalloonExpr()\n'; \
	printf '  let macros = {\n'; \
	$(CC) -E -dM $(APP_SOURCE_INCDIRS) $(APP_SYSTEM_INCDIRS) $(APP_MACROS) $^ | \
	sort -uk 1,2 | \
	sed -e 's,\\,\\\\,g; s,",\\",g' | \
	while read -r define macro repl; do \
	  case $$macro in \
	  (*) printf '\\ \47%s\47:\"%s\\n%s\",\n' "$${macro%%(*}" "$$macro" "$$repl"; \
	  esac; \
	done; \
	printf '\\ }\n'; \
	printf '  return get(macros, v:beval_text, \47\47)\n'; \
	printf 'endfunction\n'; \
	printf 'set balloonexpr=MyBalloonExpr()\n'; \
	printf 'set ballooneval\n'; \
	printf 'set balloonevalterm\n'; \
	} > $@

# make types.vim: create syntax coloring info for types in the code.
#
types.vim: $(PREPROCESSED)
	@$(CTAGS) --language-force=c --c-kinds=stu -o- $^ | \
	  awk 'BEGIN {print "autocmd Syntax * syntax keyword Type"} \
	             {print "\\ " $$1}' | uniq > $@


# vim: set syntax=make noexpandtab tabstop=8 sw=2:
