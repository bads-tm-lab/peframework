CC := g++
srcdir := $(CURDIR)/../src
objdir := $(CURDIR)/../obj/linux
sources := $(shell find $(srcdir) -name "*.cpp")
headers := $(shell find $(srcdir) -name "*.h") $(shell find $(srcdir) -name "*.hxx")
objects := $(patsubst $(srcdir)/%,$(objdir)/%.o,$(sources))
LIBMAKE = ar
INCLUDE := \
    -I$(CURDIR)/../../eirrepo/ \
    -I$(CURDIR)/../include/ \

main : $(objects) $(headers) ; mkdir -p ../lib/linux/ ; \
    $(LIBMAKE) rcs ../lib/linux/libpeframework.a $(objects)

$(objdir)/%.o : $(srcdir)/% ; \
    mkdir -p $(dir $@) ; \
    $(CC) -O3 -o $(objdir)/$(patsubst $(objdir)/%.o,%.o,$@) -c $< -Wno-invalid-offsetof $(INCLUDE) ;

clean : ; \
    rm -rf $(objdir)

print-%: ; @echo '$*=$($*)'