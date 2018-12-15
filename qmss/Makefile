# Target  name
TARGET = 
LIBA = libqmss.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib
# Libraries

INC += -I$(PROJBASE)/cm/inc
ifeq ($(NEW_QMSS_DRIVER), FALSE)
LIBS = $(PROJBASE)/cm/lib/libnetcp.a
else
LIBS = $(PROJBASE)/cm/lib/libnetcp_new.a
endif

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))

include $(PROJBASE)/cm/global.mk
CFLAGS += -DOS_LINUX

# Targets
all: $(LIBA)

# Targets
$(LIBA): buildrepo $(OBJS)
	$(AR) x $(LIBS) 
	@mv *.o $(OBJDIR)
	@mv *.ov7A $(OBJDIR)
	$(AR) q $@ $(OBJDIR)/*.*
	@mkdir -p $(LIBDIR)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)

clean:
	rm -rf $(LIBDIR) $(INCDIR) $(OBJDIR) $(SRCDIR)/*.o
	
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


