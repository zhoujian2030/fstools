# Target  name
LIBA = libcm.a

# Directories
OBJDIR = obj
SRCDIR = src
INCDIR = inc
LIBDIR = lib

# Libraries
INC += 
LIBS += 

# Files and folders
SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.c $(SRCDIR)/*.cc)
OBJS := $(subst $(SRCDIR)/, $(OBJDIR)/, $(SRCS:.cpp=.o))

#DEPOBJS := $(PROJBASE)/cm/obj/*.o

include global.mk
ifeq ($(SCTP_SUPPORT), TRUE)
CFLAGS += -DSCTP_SUPPORT
endif

all: $(LIBA)

$(LIBA): buildrepo $(OBJS)
	$(AR) q $@ $(OBJS)
	@mv $(LIBA) $(LIBDIR)
	@mkdir -p $(INCDIR)
	@cp $(SRCDIR)/*.h $(INCDIR)
	@sh ./postbuild.sh libcm

clean:
	rm -rf $(INCDIR) $(OBJDIR) $(SRCDIR)/*.o
	
install:
	# nothing to do
	
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
