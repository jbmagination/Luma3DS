SUBFOLDERS	:=	loader sm pm pxi rosalina
CXIS		:=	$(foreach dir, $(SUBFOLDERS), $(dir)/$(dir).cxi)

.PHONY:	all clean $(SUBFOLDERS)

all:	sysmodules.bin

clean:
	@$(foreach dir, $(SUBFOLDERS), $(MAKE) -C $(dir) clean &&) true
	@rm -rf sysmodules.bin

sysmodules.bin:	$(SUBFOLDERS)
	@cat $(CXIS) > $@
	@echo built... $(notdir $@)

$(SUBFOLDERS):
	@$(MAKE) -C $@ all
