

ORIGIN_LINKS = -lpigpio -lpthread -lm
PLOT_LINKS   = -lncurses
TEST_LINKS   = -lpigpio -lpthread -lm
#FLAGS =
FLAGS = -g -Werror -Wreturn-type -Wuninitialized

.DEFAULT_GOAL := origin

ORIGIN_SOURCES = $(wildcard */*.c .origin/*.c)
ORIGIN_OBJECTS = $(ORIGIN_SOURCES:%.c=compiler/%.o)
ORIGIN_HEADERS = $(ORIGIN_OBJECTS:%.o=%.d)

PLOT_SOURCES   = $(wildcard */*.c .plot/*.c)
PLOT_OBJECTS   = $(PLOT_SOURCES:%.c=compiler/%.o)
PLOT_HEADERS   = $(PLOT_OBJECTS:%.o=%.d)

TEST_SOURCES = $(wildcard */*.c .test/*.c)
TEST_OBJECTS = $(TEST_SOURCES:%.c=compiler/%.o)
TEST_HEADERS = $(TEST_OBJECTS:%.o=%.d)

origin: $(ORIGIN_OBJECTS)
	gcc $(FLAGS) -o origin.x $(ORIGIN_OBJECTS) $(ORIGIN_LINKS)

plot: $(PLOT_OBJECTS)
	gcc $(FLAGS) -o plot.x $(PLOT_OBJECTS) $(PLOT_LINKS)

test: $(TEST_OBJECTS)
	gcc $(FLAGS) -g -o test.x $(TEST_OBJECTS) $(TEST_LINKS)

-include $(ORIGIN_HEADERS) $(PLOT_HEADERS) $(TEST_HEADERS)

compiler/%.o: %.c
	@mkdir -p $(dir $(basename $@))
	gcc -o $@ -MMD -c $(FLAGS) $<

parser: $(wildcard parser/*.y parser/*.l parser/trick.nil)
	cd parser && yacc -d -t -Dparse.trace parser.y && lex parser.l && cd ..

clean:
	rm -rf compiler                             # blow away compiled objects
	rm -f *.x                                   # blow away binaries
	rm -f gmon.out                              # blow away profiling outputs
	touch data/empty~ && mv -f */*~ .backups/   # move emacs backups

wipe:
	rm -f auto.log
	rm -f logs/*.log
