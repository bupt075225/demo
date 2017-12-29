ifneq ($(KERNELRELEASE),)
    # kbuild将<module_name>.c编译成<module_name>.o,然后
	# 链接成<module_name>.ko.
	# 如果有多个源文件,需要额外列出这些文件:
	#     <module_name>-y := <src1>.o <src2>.o ...
    obj-m := netl.o
else
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
endif

# ccflags-y指定$(CC)的编译选项
ccflags-y := -g

# 构建外部模块的命令:
#     make -C <path_to_kernel_src> M=$PWD [target]
# 1."M=<dir>"选项告诉kbuild构建一个外部模块
# 2.modules是构建外部模块的默认target;
#   modules_install: 安装模块
#   clean: 删除模块目录下产生的文件
#   help: 列出可用的所有targets
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
