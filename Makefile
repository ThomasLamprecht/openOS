all: kernel user

kernel:
	$(MAKE) -C kernel

user:
	$(MAKE) -C user

.PHONY: all kernel user

tidy:
	rm kernel/*.o
	rm kernel/asm/*.o
	rm kernel/boot/*.o
	rm kernel/init/*.o
	rm user/*.o
	rm user/lib/*.o
