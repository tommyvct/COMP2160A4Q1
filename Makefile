# -----------------------------------------
# PROJECT: ObjectManager
# AUTHOR : Tommy Wu (7852291)
# -----------------------------------------

#---------------------------------------------------------------------
# C++ COMPILER, COMPILER FLAGS, AND TARGET PROGRAM NAME
#---------------------------------------------------------------------
TARGET1  =./bin/ObjectManager.out
DIR_SRC  =./src/
DIR_OBJS =./.objects
CC       =clang
CCFLAGS  =-Wall -DNDEBUG 


#---------------------------------------------------------------------
# FILES
#---------------------------------------------------------------------
# We list all files here to make our life easier in specifying dependencies
# \ means the line is not ended. Normally HEADER=header1.h header2.h
# But we want to separate lines so they look nicer.
HEADERS=./src/ObjectManager.h

SOURCES=./src/main.c ./src/ObjectManager.c

MAIN_OBJECT=$(DIR_OBJS)/main.o

OTHER_OBJECT = $(DIR_OBJS)/ObjectManager.o


#---------------------------------------------------------------------
# COMPILING RULES
#---------------------------------------------------------------------
# First rule is default, that is, if you don't specify specific rule, makefile will use the first rule.
default: $(TARGET1)

# Link compiled objects and create an executable binary.
# It requires to have directory for objects and to compile objects
$(TARGET1): check_headers make_directory  $(MAIN_OBJECT) $(OTHER_OBJECT)
	$(CC)  $(MAIN_OBJECT) $(OTHER_OBJECT) -o $@



check_headers: $(HEADERS)

make_directory:
	mkdir -p $(DIR_OBJS)/
	mkdir -p ./bin/

# Compile C files
$(DIR_OBJS)/%.o: $(DIR_SRC)%.c
	$(CC) $(CCFLAGS) -c $? -o $@

# Clean removes every object and binary
clean:
	rm -rf $(TARGET1) $(MAIN_OBJECT) $(DIR_OBJS)/


#---------------------------------------------------------------------
# END OF MAKEFILE
#---------------------------------------------------------------------
