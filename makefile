
MKCWD=mkdir -p $(@D)

PROJECT_NAME = craytracer

CC = clang
CFLAGS = -mavx -msse4 -msse3 -msse2 -msse -ansi -Ofast -g -Isrc/  \
	-std=c89 -Wall -Wextra  \
	-pedantic -Wmissing-prototypes -Wstrict-prototypes \
    -Wold-style-definition -Werror -flto

BUILD_DIR = build

# source files

CFILES = $(wildcard src/*.c)
HFILES = $(wildcard src/*.h)
OFILES = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(CFILES))

OUTPUT = build/$(PROJECT_NAME).elf


$(OUTPUT): $(OFILES)
	@$(MKCWD)
	@echo "[ $@ ] $^"
	gcc $(CFLAGS) -lpthread -lm -lSDL2 -o $@ $^

$(BUILD_DIR)/%.o: src/%.c 
	@$(MKCWD)
	@echo "[ $@ ] $^"
	gcc $^ -c -o $@  $(CFLAGS)

run: $(OUTPUT)
	$(OUTPUT)

all: $(OUTPUT)

clean:
	rm -rf build/
