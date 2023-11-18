KENV = $(shell uname -r)

obj-m += MeltdownKernel.o


all:
	make -C /lib/modules/$(KENV)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KENV)/build M=$(PWD) clean
