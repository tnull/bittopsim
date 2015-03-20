CC = clang++
CFLAGS = -O2 -Wall -g -std=c++11 -Wno-c++11-extensions -pedantic -W -Wextra
SRCS = bittopsim.cpp node.cpp
OBJS = $(SRCS:.cpp=.o)
MAIN = bittopsim

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) 
.cpp.o:
	$(CC) $(CFLAGS) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
