#    Never export any environment variables, unless exported explicitly.
#    This keeps the environment small and surprises low.
#    Make sure that utilities have no hidden dependency on locales.
#    For example, date(1) output depends on LANG.
unexport                  # Never export to sub-makes.
unexport C_INCLUDE_PATH   # Remove from shell environment; confuses gcc.
export LANG   = C.UTF-8
export LC_ALL = C.UTF-8

.DEFAULT_GOAL = install

TOP_LEVEL_TARGETS = lscp
TOP_LEVEL_TARGETS += txttopng
TOP_LEVEL_TARGETS += srctohex
TOP_LEVEL_TARGETS += hextobdf
TOP_LEVEL_TARGETS += gallant.bdf
TOP_LEVEL_TARGETS += gallant.fnt


.PHONY: all
all: $(TOP_LEVEL_TARGETS)

lscp: lscp.o

srctohex: srctohex.o

hextobdf: hextobdf.o

tsttopng: tsttopng.o

gallant.bdf: gallant.hex
	./hextobdf < $^ > $@

gallant.hex: gallant.src
	./srctohex < $^ > $@

gallant.fnt: gallant.hex
	  vtfontcvt -v -o $@ $^

.PHONY: install
install: gallant.bdf gallant.fnt
	cp gallant.bdf ~/.fonts
	cd ~/.fonts && mkfontdir && xset fp rehash
	if test $$(uname -s) = FreeBSD; then \
	  if test -w /dev/ttyv7; then \
	    vidcontrol -f gallant.fnt < /dev/ttyv7; \
	  fi; \
	fi

.PHONY: images
images:
	./make-images.sh

# Find un-sorted codepoints in gallant.src.
#
.PHONY: check
check:
	@grep ^STARTCHAR gallant.src | sort -c

README.html: README.md
	comrak --gfm --syntax-highlighting base16-ocean.light $^ > $@

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
APP_WARNS  += -Wno-unused
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

lscp: lscp.o
	$(CC) -o $@ $(APP_LIBDIRS) -luninameslist -lunistring $^

hextobdf: hextobdf.o
	$(CC) -o $@ $^

srctohex: srctohex.o
	$(CC) -o $@ $^

txttopng: txttopng.o
	$(CC) -o $@ $(APP_LIBDIRS) -lpng $^

lint: txttopng.c lscp.c hextobdf.c srctohex.c
	@for c in $^; do flexelint lint.lnt $$c; done

# X11 in x.out:
# BDF Error on line 114938: char 'U+10000' has encoding too large (65536)

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
.SUFFIXES: .asm .c .elf .i .o .p .s

#   How to compile a C source to an object file.
#
%.o: %.c
	$(CC) -c $(APP_CFLAGS) $(APP_WARNS) $(APP_SOURCE_INCDIRS) $(APP_MACROS) -o $@ $<

#   How to compile a C source to an assembler file.
#
%.s: %.c
	$(CC) -S $(APP_CFLAGS) $(APP_WARNS) $(APP_SOURCE_INCDIRS) $(APP_MACROS) -o $@ $<

#   How to pre-process a C source file with the compiler and save result in a *.i file.
#
%.i: %.c
	$(CC) -E $(APP_CFLAGS) $(APP_WARNS) $(APP_SOURCE_INCDIRS) $(APP_MACROS) -o $@ $<

#   How to pre-process a C source file with FlexeLint and save result in a *.p file.
#
%.p: %.c
	$(FLINT) -p $(LNT) $(APP_INCDIRS) $(APP_MACROS) $< > $@

#   How to preprocess with the compiler, without # linemarkers and indented.
#
%.pp: %.c
	@mkdir -p $(@D)
	$(CC) -E -P -DCOMPILING=1 $(APP_CFLAGS) $(APP_MACROS) $(APP_SOURCE_INCDIRS) $< | \
	gindent -st - \
	  --ignore-profile \
	  -T  int8_t -T  int16_t -T  int32_t -T  int64_t -T float32_t \
	  -T uint8_t -T uint16_t -T uint32_t -T uint64_t -T float64_t \
	  --k-and-r-style \
	  --ignore-newlines \
	  --braces-on-func-def-line \
	  --dont-line-up-parentheses \
	  --continuation-indentation1 \
	  --indent-level1 \
	  --line-length9999 \
	  --no-tabs \
	  --tab-size1 > $@

#   How to extract functions from *.pp file.
#
%.func: %.pp
	awk '/^[[:alpha:]_].*) {$$/,/^}$$/' $< > $@

################################################################################
#        _   _      _                   _____                    _             #
#       | | | | ___| |_ __   ___ _ __  |_   _|_ _ _ __ __ _  ___| |_ ___       #
#       | |_| |/ _ \ | '_ \ / _ \ '__|   | |/ _` | '__/ _` |/ _ \ __/ __|      #
#       |  _  |  __/ | |_) |  __/ |      | | (_| | | | (_| |  __/ |_\__ \      #
#       |_| |_|\___|_| .__/ \___|_|      |_|\__,_|_|  \__, |\___|\__|___/      #
#                    |_|                              |___/                    #
#                                                                              #
################################################################################

#------------------------------------------------------------------------------#
#                                    Clean                                     #
#------------------------------------------------------------------------------#

.PHONY: clean
clean:
	git clean -fdx

#------------------------------------------------------------------------------#
#                          Tags - Create tags for vi                           #
#------------------------------------------------------------------------------#

# Create tags from the actual files compiled, and the actually included
# headers. Examine the preprocessor output's '# LINE "FILE"' directives.

#PREPROCESSED = $(APP_C_SOURCE:.c=.i)
PREPROCESSED = lscp.i txttopng.i hextobdf.i srctohex.i

# make tags: create vi tags file.
#
CTAGS = jexctags
.PHONY: tags
tags: $(PREPROCESSED)
	@awk '/^# / {print $$3}' $^ | sort -u | grep -v \< | tr -d \" > list
	@$(CTAGS) -L list -f $@ \
	         --regex-c='/\<(REQ_[[:upper:]_]*([[:digit:]_]{7}))\>/\2/'
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
tooltips.vim: lscp.c txttopng.c
	{ \
	printf "function! MyBalloonExpr()\n"; \
	printf "  let macros = {\n"; \
	$(CC) -E -dM $(APP_SOURCE_INCDIRS) $(APP_SYSTEM_INCDIRS) $(APP_MACROS) $^ | \
	sort -uk 1,2 | \
	sed -e 's,\\,\\\\,g; s,",\\",g' | \
	while read -r define macro repl; do \
	  case $$macro in \
	  (HASH) printf "\\ 'HASH':\"HASH\\\\n0x12345678u\",\n";; \
	  (*) printf "\\ '%s':\"%s\\\\n%s\",\n" "$${macro%%(*}" "$$macro" "$$repl"; \
	  esac; \
	done; \
	printf "\\ }\n"; \
	printf "  return get(macros, v:beval_text, '')\n"; \
	printf "endfunction\n"; \
	printf "set balloonexpr=MyBalloonExpr()\n"; \
	printf "set ballooneval\n"; \
	printf "set balloonevalterm\n"; \
	} > $@

# make types.vim: create syntax coloring info for types in the code.
#
types.vim: $(PREPROCESSED)
	@$(CTAGS) --language-force=c --c-kinds=stu -o- $^ | \
	  awk 'BEGIN {print "autocmd Syntax * syntax keyword Type"} \
	             {print "\\ " $$1}' | uniq > $@


# vim: set syntax=make noexpandtab tabstop=8 sw=2:
