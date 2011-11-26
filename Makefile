all: kernel user

kernel:
	$(MAKE) -C kernel

user:
	$(MAKE) -C user

.PHONY: all kernel user