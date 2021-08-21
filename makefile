
MKCWD=mkdir -p $(@D)

PROJECT_NAME = craytracer

CC = gcc
CFLAGS = -mavx -msse4 -msse3 -msse2 -msse -ansi -O3 --analyzer -g -Isrc/  \
	-lpthread -lm -lSDL2 -std=c89 -Wall -Wextra  \
	-pedantic -Wmissing-prototypes -Wstrict-prototypes \
    -Wold-style-definition

BUILD_DIR = build

# source files

CFILES = $(wildcard src/*.c)
HFILES = $(wildcard src/*.h)
OFILES = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(CFILES))

OUTPUT = build/$(PROJECT_NAME).elf


$(OUTPUT): $(OFILES)
	@$(MKCWD)
	@echo "[ $@ ] $^"
	@$(CC) -o $@ $^ $(CFLAGS)

$(BUILD_DIR)/%.o: src/%.c 
	@$(MKCWD)
	@echo "[ $@ ] $^"
	@$(CC) $^ -c -o $@  $(CFLAGS)

run: $(OUTPUT)
	$(OUTPUT)

all: $(OUTPUT)

clean:
	rm -rf build/
