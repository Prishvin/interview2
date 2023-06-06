# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -g -Wall

# define any directories containing header files other than /usr/include
INCLUDES = 

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -L/usr/local/lib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use -llibx
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lapr-1 -ljansson

# define the C source files
SRCS = common.c stack.c  reader.c hash.c tlv.c

# define the C object files 
# (This uses Suffix Replacement within a macro:
#   replace %.c with %.o)
OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = reader

.PHONY: depend clean

all:    $(MAIN)
	@echo  $(MAIN) has been compiled

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file in this case) and $@: the name of the target of the rule
# (a .o file in this case). The -c flag says to generate the object file, 
# the -o $@ says to put the output of the compilation in the file named 
# on the left side of the :, the $< is the first item in the dependencies list
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it 
