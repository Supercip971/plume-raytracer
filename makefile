
MKCWD=mkdir -p $(@D)

PROJECT_NAME = craytracer

CC = gcc

# for TCC
# CFLAGS =  -DSDL_DISABLE_IMMINTRIN_H=0 -Isrc/ -pedantic  -O3  -std=gnu89
 CFLAGS =  -march=native -ansi -Ofast  -Isrc/  \
 	-std=gnu89 -Wall -Wextra  \
 	-pedantic -Wmissing-prototypes -Wstrict-prototypes \
     -Wold-style-definition -Werror -flto      \
 	  -ffast-math  -DUSE_INTRINSIC=0


BUILD_DIR = build

# source files

CFILES = $(wildcard src/*.c)  $(wildcard src/*/*.c)
HFILES = $(wildcard src/*.h)  $(wildcard src/*/*.h)
OFILES = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(CFILES))

OUTPUT = build/$(PROJECT_NAME).elf


$(OUTPUT): $(OFILES)
	@$(MKCWD)
	@echo "[ $@ ] $^"
	$(CC) $(CFLAGS) -lc -lpthread -lm -lSDL2 -o $@ $^ 

$(BUILD_DIR)/%.o: src/%.c $(HFILES)
	@$(MKCWD)
	@echo "[ $@ ] $<"
	$(CC) $< -c -o $@  $(CFLAGS)

run: $(OUTPUT)
	$(OUTPUT)

all: $(OUTPUT)

clean:
	rm -rf build/

call_graph: 
	gprof   $(OUTPUT) ./gmon.out >> ./build/gprof.txt
	gprof2dot ./build/gprof.txt > ./build/call_graph.txt
	xdot ./build/call_graph.txt
