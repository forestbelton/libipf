ifeq ($(OS),Windows_NT)
	EXESUFFIX = .exe
else
	EXESUFFIX
endif

CC        = $(CROSS_COMPILE)g++
STRIP     = $(CROSS_COMPILE)strip
TARGET    = unipf$(EXESUFFIX)
SRCS	  = unipf.cpp
OBJS	  = $(SRCS:%.cpp=%.o)
DEPS	  = $(SRCS:%.cpp=%.d)
INCLUDE_DIRS =  . ../include

LDFLAGS   = -L../ -lipf -lz
CFLAGS    = -Wall -O2 $(addprefix -I,$(INCLUDE_DIRS))
ifeq ($(OS),Windows_NT)
	CFLAGS := $(CFLAGS) -static -static-libgcc -static-libstdc++
endif

all: $(TARGET)

-include $(DEPS)

clean: 
	rm -f $(OBJS) $(DEPS) $(TARGET) *~ *.scc *.vcproj *.vspscc ipf$(EXESUFFIX)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(DEBUGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	rm -f ipf$(EXESUFFIX)
	cp unipf$(EXESUFFIX) ipf$(EXESUFFIX)

.cpp.o:
	$(CC) -c -MMD -MP -o $@ $(CFLAGS) $(DEBUGS) $<

