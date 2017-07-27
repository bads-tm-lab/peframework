CC := g++
srcdir := $(CURDIR)/../src
objdir := $(CURDIR)/../obj/linux
sources := $(wildcard $(srcdir)/**.cpp)
headers := $(wildcard $(srcdir)/**.h)
objects := $(patsubst $(srcdir)/%.cpp,$(objdir)/%.o,$(sources))
LIBMAKE = ar
INCLUDE := \
    -I$(CURDIR)/../../eirrepo/ \
    -I$(CURDIR)/../include/ \

main : $(objects) $(headers) ; mkdir -p ../lib/linux/ ; \
    $(LIBMAKE) rcs ../lib/linux/libpeframework.a $(objects)

$(objdir)/%.o : $(srcdir)/%.cpp ; \
    mkdir -p $(dir $@) ; \
    $(CC) -o $(objdir)/$(patsubst $(objdir)/%.o,%.o,$@) -c $< -Wno-invalid-offsetof $(INCLUDE) ;

print-%: ; @echo '$*=$($*)'