# Complier -fPIC is mandatory when building dynamic lib *.so
CC = arm-linux-gcc
CXX = arm-linux-g++
AR = arm-linux-ar

# For debug load, DON'T define NDEBUG 
#CFLAGS = -Wall -g -O2 -DNDEBUG -fPIC
CFLAGS = -Wall -g -O2 -fPIC
ifeq ($(PLATFORM), arm)
CFLAGS = -Wall -g -Os -fPIC
endif

ifeq ($(NEW_QMSS_DRIVER), TDD_TRUE)
CFLAGS += -DTWO_CARRIER -DTDD
else

ifeq ($(NEW_QMSS_DRIVER), FDD_TRUE)
CFLAGS += -DTWO_CARRIER -DFDD
endif 

endif

LFLAGS = -lrt

.%.d: %.cpp
	$(CXX) $(INC) -MM $< > $@
	@$(CXX) $(INC) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(INC) $(CFLAGS) -c $< -o $@

.%.d: %.c
	$(CC) $(INC) -MM $< > $@
	@$(CC) $(INC) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
                
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(INC) $(CFLAGS)  -c $< -o $@
	
%.o: %.cc
	$(CXX) $(INC) $(CFLAGS)  -c $<
