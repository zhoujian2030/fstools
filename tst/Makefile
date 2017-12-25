# Target  name
TARGET = tst

# Directories
OBJDIR = obj
SRCDIR = src
DEPDIR = dep
BINDIR = bin
# Libraries

INC += -I$(PROJBASE)/cm/inc -I$(PROJBASE)/qmss/inc
LIBS = $(PROJBASE)/qmss/lib/libqmss.a $(PROJBASE)/cm/lib/libcm_arm.a 

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))

include $(PROJBASE)/cm/global.mk
CFLAGS += -DOS_LINUX

all: $(TARGET)

# Targets
$(TARGET): buildrepo $(OBJS)
	$(CXX) $(OBJS) -o $@ -lrt -pthread $(LIBABOOST) $(LIBAGTEST) $(LIBS)
	@mv $(TARGET) $(OBJDIR)

clean:
	rm -rf $(BINDIR) $(OBJDIR) $(SRCDIR)/*.o $(DEPDIR)

install:
	mkdir -p $(BINDIR)
	cp $(OBJDIR)/$(TARGET) $(BINDIR)
	
buildrepo:
	@$(call make-repo)

# Create obj directory structure
define make-repo
	mkdir -p $(OBJDIR)
	for dir in $(SRCDIRS); \
	do \
		mkdir -p $(OBJDIR)/$$dir; \
	done
endef


