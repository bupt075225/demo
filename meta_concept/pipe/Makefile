CC=gcc
SRCS_D=create_pipe.c
SRCS_E=exec_cmd.c
OBJS_D=$(SRCS_D:.c=.o)
OBJS_E=$(SRCS_E:.c=.o)

CFLAGS+=-g
PROGRAMES=create_pipe exec_cmd
LIBS=

all: $(PROGRAMES)

create_pipe: .depend_d $(OBJS_D)
	$(CC) $(OBJS_D) $(LIBS) -o $@

exec_cmd: .depend_d $(OBJS_E)
	$(CC) $(OBJS_E) $(LIBS) -o $@

#使用include关键字将Makefile文件.depend_d包含进来
ifeq (.depend_d,$(wildcard .depend_d))
-include .depend_d
endif

#将源文件编译成目标文件
#这是一个模式规则(相对隐含规则),%表示一个多或多个任意字符
#$@表示所有的目标,$<表示所有的依赖目标
%.o: %.c Makefile
	$(CC) -c -o $(@) $(CFLAGS) $(<)

#使用编译器来自动寻找源文件包含的头文件,并生成一个依赖关系
#生成的依赖关系放在名为.depend_d的Makefile文件中
.depend_d:
	$(CC) -M $(CFLAGS) $(SRCS_D) > $(@)

clean:
	rm -f $(PROGRAMES) *.o .depend*
