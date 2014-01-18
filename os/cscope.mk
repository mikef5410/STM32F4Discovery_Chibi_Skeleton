#
# CSCOPE
#
cscope: CSCOPE_DIRS := $(patsubst %,-I %,$(INCDIR))
cscope: CSCOPE_INCLUDES := $(patsubst %,-s %,$(INCDIR))
cscope: CSCOPE_FILES := $(shell find $(INCDIR) -name "*.h" -print)
cscope: CSCOPE_FILES += $(CSRC) $(CPPSRC) $(ACSRC) $(ACPPSRC) $(TCSRC) $(TCPPSRC) $(ASMSRC)

cscope:
	cscope -bquk $(CSCOPE_INCLUDES) $(CSCOPE_FILES) 

clean::
	rm -f cscope.in.out cscope.out cscope.po.out
