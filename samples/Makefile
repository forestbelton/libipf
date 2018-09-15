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

CFLAGS    = -Wall -O2 $(addprefix -I,$(INCLUDE_DIRS))
LDFLAGS   = -L../ -lipf


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

