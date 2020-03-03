

ORIGIN_LINKS = -lpigpio -lncurses -lpthread -lm
TEST_LINKS   = -lpigpio -lncurses -lpthread -lm
#FLAGS =
FLAGS = -g -Werror -Wreturn-type -Wuninitialized

.DEFAULT_GOAL := origin

ORIGIN_SOURCES = $(wildcard */*.c .origin/*.c)
ORIGIN_OBJECTS = $(ORIGIN_SOURCES:%.c=compiler/%.o)
ORIGIN_HEADERS = $(ORIGIN_OBJECTS:%.o=%.d)

TEST_SOURCES = $(wildcard */*.c .test/*.c)
TEST_OBJECTS = $(TEST_SOURCES:%.c=compiler/%.o)
TEST_HEADERS = $(TEST_OBJECTS:%.o=%.d)

origin: $(ORIGIN_OBJECTS)
	gcc $(FLAGS) -o origin.x $(ORIGIN_OBJECTS) $(ORIGIN_LINKS)

test: $(TEST_OBJECTS)
	gcc $(FLAGS) -g -o test.x $(TEST_OBJECTS) $(TEST_LINKS)

-include $(ORIGIN_HEADERS) $(TEST_HEADERS)

compiler/%.o: %.c
	@mkdir -p $(dir $(basename $@))
	gcc -o $@ -MMD -c $(FLAGS) $<

parser: $(wildcard parser/*.y parser/*.l parser/trick.nil)
	cd parser && yacc -d -t -Dparse.trace parser.y && lex parser.l && cd ..

count:
	@bash .count.sh

clean:
	rm -rf compiler/* compiler/.[a-Z]+.*        # blow away compiled objects
	rm -f *.x                                   # blow away binaries
	rm -f gmon.out                              # blow away profiling outputs
	touch data/empty~ && mv -f */*~ .backups/   # move emacs backups

wipe:
	rm -f auto.log
	rm -f logs/*.log
