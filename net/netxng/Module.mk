# Module.mk for xrd module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Lukasz Janyst 11/01/2013

MODNAME      := netxng
MODDIR       := $(ROOT_SRCDIR)/net/$(MODNAME)
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

NETXNGDIR      := $(MODDIR)
NETXNGDIRS     := $(MODDIR)/src
NETXNGDIRI     := $(MODDIR)/inc

##### libNetXNG #####
NETXNGL        := $(MODDIRI)/LinkDef.h
NETXNGDS       := $(call stripsrc,$(MODDIRS)/G__NetXNG.cxx)
NETXNGDO       := $(NETXNGDS:.cxx=.o)
NETXNGDH       := $(NETXNGDS:.cxx=.h)

NETXNGH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
NETXNGS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
NETXNGO        := $(call stripsrc,$(NETXNGS:.cxx=.o))

NETXNGDEP      := $(NETXNGO:.o=.d) $(NETXNGDO:.o=.d)

NETXNGLIB      := $(LPATH)/libNetXNG.$(SOEXT)
NETXNGMAP      := $(NETXNGLIB:.$(SOEXT)=.rootmap)

NETXNGINCEXTRA := $(XRDINCDIR:%=-I%)
NETXNGLIBEXTRA += $(XRDLIBDIR) -lXrdUtils -lXrdCl

ALLHDRS      += $(patsubst $(MODDIRI)/%.h,include/%.h,$(NETXNGH))
ALLLIBS      += $(NETXNGLIB)
ALLMAPS      += $(NETXNGMAP)
INCLUDEFILES += $(NETXDEP)

##### local rules #####
.PHONY:         all-$(MODNAME) clean-$(MODNAME) distclean-$(MODNAME)

include/%.h:    $(NETXNGDIRI)/%.h
		cp $< $@

$(NETXNGLIB):     $(NETXNGO) $(NETXNGDO) $(ORDER_) $(MAINLIBS) $(NETXNGLIBDEP) \
                $(XRDNETXNGD)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libNetXNG.$(SOEXT) $@ "$(NETXNGO) $(NETXNGDO)" \
		   "$(NETXNGLIBEXTRA)"

$(NETXNGDS):      $(NETXNGH) $(NETXNGL) $(XROOTDMAKE) $(ROOTCINTTMPDEP) $(XRDPLUGINS)
		$(MAKEDIR)
		@echo "Generating dictionary $@..."
		$(ROOTCINTTMP) -f $@ -c $(NETXNGINCEXTRA) $(NETXNGH) $(NETXNGL)

$(NETXNGMAP):     $(RLIBMAP) $(MAKEFILEDEP) $(NETXNGL)
		$(RLIBMAP) -o $@ -l $(NETXNGLIB) -d $(NETXNGLIBDEPM) -c $(NETXNGL)

all-$(MODNAME): $(NETXNGLIB) $(NETXNGMAP)

clean-$(MODNAME):
		@rm -f $(NETXNGO) $(NETXNGDO)

clean::         clean-$(MODNAME)

distclean-$(MODNAME): clean-$(MODNAME)
		@rm -f $(NETXNGDEP) $(NETXNGDS) $(NETXNGDH) $(NETXNGLIB) $(NETXNGMAP)

distclean::     distclean-$(MODNAME)

$(NETXNGO) $(NETXNGDO): CXXFLAGS += $(NETXNGINCEXTRA)
