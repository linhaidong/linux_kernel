#linux 内核模块编译

>1.各个要素:内核目录，目标文件，源文件
>2.具体过程

1.各个要素
1.1 内核目录
why:
内核模块的编译，在内核的整体框架(makfile和内核源文件)下编译，
因此需要指定内核的路径。

how:
linux源码的目录有两个，分别为:
"/lib/modules/$(shell uname -r)/build"
"/usr/src/linux-header-$(shell uname -r)/"
usr目录下那个源代码一般是我们自己下载后解压的，而lib目录下的则是在编译时自动copy过去的，
两者的文件结构完全一样，因此有时也将内核源码目录设置成/usr/src/linux-header-$(shell uname -r)/
关于内核源码目录可以根据自己的存放位置进行修改。

1.2 目标文件和源文件
指定目标文件:obj-m := moudle1_name.o,  moudle2_name.o moudle3_name.o
指定源文件: moudle1-objs := file1.o file2.o file3.o

obj-m是个makefile变量，他的值可以是一串.o文件的列表。
列表中的每一项，代表一个模块。
其实，我们可以通过一个makefile编译出多个模块。
例如，假设hello目录下存放了多个模块的C文件，别是hello、hello2、hello3。
hello模块的构成：main.c  a.c  b.c
hello2模块的构成：main2.c  a2.c  b2.c
hello3模块的构成：hello3.c
此时，Makefile写成如下形式即可。
obj-m := hello.o hello2.o hello3.o
hello-objs := main.o a.o b.o
hello2-objs := main2.o a2.o b2.o



2.具体的编译过程
2.1 编译过程
编译命令:
    make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
    这就是编译模块了：
    首先改变目录到-C选项指定的位置（即内核源代码目录），其中保存有内核的顶层makefile；
    M=选项让该makefile在构造modules目标之前返回到模块源代码目录；
    然后，modueles目标指向obj-m变量中设定的模块；在上面的例子中，我们将该变量设置成了hello.o。
具体过程:
    (1) 第一次进来的时候，宏“KERNELRELEASE”未定义，因此进入 else；
    (2) 记录内核路径，记录当前路径；
         由于make 后面没有目标，所以make会在Makefile中的第一个不是以.开头的目标作为默认的目标执行。默认执行all这个规则
    (3) make -C $(KDIR) M=$(PWD) modules
         -C 进入到内核的目录执行Makefile ，在执行的时候KERNELRELEASE就会被赋值，M=$(PWD)表示返回当前目录，
        再次执行makefile，modules 编译成模块的意思
         所以这里实际运行的是
         make -C /lib/modules/2.6.13-study/build M=/home/fs/code/1/module/hello/ modules

    (4) 再次执行该makefile，KERNELRELEASE就有值了，就会执行obj-m:=hello.o
         obj-m：表示把hello.o 和其他的目标文件链接成hello.ko模块文件，编译的时候还要先把hello.c编译成hello.o文件

2.2 使用过程
    make 
    make clean
    #insmod hello.ko
    #rmmod  hello 
2.3 日志过程
    #dmesg
    [  382.209386] hello world module init!
    [  382.209389] hello world module init!
    [  642.519772] hello world module exit!
