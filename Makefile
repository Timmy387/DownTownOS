CF = -ansi -c
ASF = -f as86
SRC = src/
OBJDIR = src/objects/
KSIZE = 7
KSTART = 3
C_SRC = \
			kernel.c 				\
			commands/commands.c 	\
			commands/handler.c 		\
			files/fileBackend.c 	\
			files/fileCmds.c		\
            memory/malloc.c 		\
			strings/pStr.c 			\
			strings/string.c 		\
			textEditor/openFile.c 	\
			textEditor/typeWriter.c \
			utils/screen.c 			\
			utils/time.c 			\
			utils/util.c
C_SOURCES := $(addprefix $(SRC), $(C_SRC))

ASM_SRC = assembly/kernel.asm assembly/asmUtils.asm
ASM_SOURCES := $(addprefix $(SRC), $(ASM_SRC))

C_OBJECTS := $(addprefix $(OBJDIR), $(C_SRC:.c=.o))
ASM_OBJECTS := $(addprefix $(OBJDIR), $(ASM_SRC:.asm=.o))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

all: disk0.img kernel

bootload:
	dd if=/dev/zero of=disk0.img bs=512 count=720
	nasm src/assembly/bootload.asm -o bootload

disk0.img: bootload
	dd if=bootload of=disk0.img bs=512 count=1 conv=notrunc


# Compile C source files
$(OBJDIR)%.o: $(SRC)%.c
	@mkdir -p $(dir $@)
	bcc $(CF) -o $@ $<

# Compile assembly source files

$(OBJDIR)%.o: $(SRC)%.asm
	@mkdir -p $(dir $@)
	nasm $(ASF) -o $@ $<

kernel: $(OBJECTS)
	ld86 -o kernel -d $(OBJECTS)
	dd if=kernel of=disk0.img bs=512 conv=notrunc seek=3
	@make rmextras

rmextras:
	@rm -f $(OBJECTS) bootload kernel

clean:
	rm -f disk0.img
	rm -f $(OBJECTS) bootload kernel
	rm -rf src/objects