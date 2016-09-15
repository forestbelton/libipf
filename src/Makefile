
CC        = $(CROSS_COMPILE)g++
STRIP     = $(CROSS_COMPILE)strip

TARGET    = libipf.so

SRCS	  = ipfcrypt.cpp ipffile.cpp libipf.cpp
OBJS	  = $(SRCS:%.cpp=%.o)
DEPS	  = $(SRCS:%.cpp=%.d)
INCLUDE_DIRS =  . ../include

CFLAGS    = -Wall -O2 -fPIC -shared $(addprefix -I,$(INCLUDE_DIRS))
LDFLAGS   = -lz


all: $(TARGET)

-include $(DEPS)

clean: 
	rm -f $(OBJS) $(DEPS) $(TARGET) *~ *.scc *.vcproj *.vspscc

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(DEBUGS) -o $@ $^ $(LDFLAGS)
#	$(STRIP) $@

.cpp.o:
	$(CC) -c -MMD -MP -o $@ $(CFLAGS) $(DEBUGS) $<

