This is a project from Thomas Lamprecht. It has been initiated in December 2011 as a research project for his mature exame. Which will be made at the "Gewerbe Oberschule Bruneck" (Technical Highschool Bruneck)

# Releases

openOS isn't really the name of the project. Since developing started we've given fruit names as Code-names in alphabetical order to the 'releases'.
Code-names 'til yet:

* Apple
* Banana
* Cranberry
* Dragon Fruit
* Elderberry
* Feijoa

# Build
execute `make` in the kernel directory to build the kernel.

The builded kernel, in elf format, could be executed with qemu like this:

`qemu -kernel kernel`

With a userland programme:

`qemu -kernel kernel -initrd ./user/test.bin`
