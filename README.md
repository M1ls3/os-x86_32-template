# os-x86_32-template

Own operating system.
Implemented:
- Write and delete text, scroll;
- Command help and clear;
- Including file system;
- Command sleep.

# Command list:
- help;
- clear;
- create (filename);
- delete (filename);
- edit (filename);
- read (filename);
- list;

# Install:
For Windows:
1. Docker + Qemu: Install qemu emulator. Install docker. Use the .\run.bat script to compile and run the OS kernel. (recommended)
2. Install Windows Subsystem for Linux (WSL). Then follow the instructions from the Linux section. You can put everything you need through the apt-get package manager, including qemu. Then just use the make command.

For Linux:
1. To install gcc, nasm, ld, qemu, depending on the distribution, you need different commands, you can find them quite easily. For systems with apt-get package manager:
- `sudo apt-get update`
- `sudo apt-get install -y gcc nasm binutils make aqemu`

For Mac:
1. Docker + Qemu
2. Install Docker.
3. Install qemu `brew install qemu`.
4. Use the `./run.sh' script to compile and run the OS kernel.
