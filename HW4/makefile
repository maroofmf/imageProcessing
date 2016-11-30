# Macros:
cc = th
qtcc = qlua
OUTPUTFOLDERS = models plots metadata

# Color Macros:
RED = \033[0;31m
NC = \033[0m
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[1;33m

# Code Execution:
all: cnn_p1

cnn_p1: setup
	@printf "${YELLOW}=====> Training and Testing model from mnist-p1-train.t7\n ${NC}"
	$(cc) cnn_p1.lua

clean:
	@printf "${RED}=====> Destroying folders\n ${NC}"
	rm -rf $(OUTPUTFOLDERS)

setup:
	@printf "${GREEN}=====> Constructing Folders\n ${NC}"
	mkdir -p $(OUTPUTFOLDERS)