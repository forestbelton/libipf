ifeq ($(OS),Windows_NT)
	LIBSUFFIX = .dll
else
	LIBSUFFIX = .so
endif

CC        = $(CROSS_COMPILE)g++
STRIP     = $(CROSS_COMPILE)strip
TARGET    = libipf$(LIBSUFFIX)
STATIC    = libipf.a
SRCS	  = ipfcrypt.cpp ipffile.cpp libipf.cpp
OBJS	  = $(SRCS:%.cpp=%.o)
DEPS	  = $(SRCS:%.cpp=%.d)
INCLUDE_DIRS =  . ../include

CFLAGS    = -Wall -O2 -fPIC -shared $(addprefix -I,$(INCLUDE_DIRS))
LDFLAGS   = -lz

all: $(TARGET) $(STATIC)

-include $(DEPS)

clean: 
	rm -f $(OBJS) $(DEPS) $(TARGET) *~ *.scc *.vcproj *.vspscc

$(STATIC): $(OBJS)
	$(AR) rcs $@ $^

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(DEBUGS) -o $@ $^ $(LDFLAGS)
#	$(STRIP) $@

.cpp.o:
	$(CC) -c -MMD -MP -o $@ $(CFLAGS) $(DEBUGS) $<

