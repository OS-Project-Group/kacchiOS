# 🍚 kacchiOS

A minimal, educational baremetal operating system designed for teaching OS fundamentals.

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/platform-x86-lightgrey.svg)]()

## 📖 Overview

kacchiOS is a fully-functional, educational bare-metal operating system built from scratch. It demonstrates core OS concepts including process management, memory allocation, and scheduling algorithms. The kernel implements a working priority-based scheduler that manages multiple processes with context switching, a dynamic memory manager, and essential I/O drivers—providing a solid foundation for understanding modern operating system design.

### Current Features

- ✅ **Multiboot-compliant bootloader** - Boots via GRUB/QEMU
- ✅ **Serial I/O driver** (COM1) - Communication via serial port
- ✅ **Memory Manager** - Dynamic memory allocation and management
- ✅ **Process Manager** - Create, manage, and terminate processes
- ✅ **Scheduler** - Priority-based process scheduling with context switching
- ✅ **Basic string utilities** - Essential string operations
- ✅ **Clean, documented code** - Easy to understand and extend

## 🚀 Quick Start

### Prerequisites

```bash
# On Ubuntu/Debian
sudo apt-get install build-essential qemu-system-x86 gcc-multilib

# On Arch Linux
sudo pacman -S base-devel qemu gcc-multilib

# On macOS
brew install qemu i686-elf-gcc
```

### Build and Run

```bash
# Clone the repository
git clone https://github.com/yourusername/kacchiOS.git
cd kacchiOS

# Build the OS
make clean
make

# Run in QEMU
make run
```

**Expected Output:**

The kernel will boot and run multiple processes managed by the scheduler:

```
========================================
    kacchiOS - Educational OS Kernel
========================================
Initializing memory manager...
Creating processes...
[Process 1 - Priority 5] Iteration running
[Process 2 - Priority 3] Iteration running
[Process 3 - Priority 3] Iteration running
[Process 1] Completed and exiting.
...
```

The scheduler manages multiple concurrent processes, demonstrating context switching and priority-based execution.

## 📁 Project Structure

```
kacchiOS/
├── src/
│   ├── boot.S          # Bootloader entry point (Assembly)
│   ├── ctxsw.S         # Context switch assembly routines
│   ├── kernel.c        # Main kernel and process management
│   ├── kernel.h        # Kernel interface
│   ├── memory.c        # Memory manager implementation
│   ├── memory.h        # Memory manager interface
│   ├── process.c       # Process management implementation
│   ├── process.h       # Process interface
│   ├── scheduler.c     # Process scheduler implementation
│   ├── scheduler.h     # Scheduler interface
│   ├── serial.c        # Serial port driver (COM1)
│   ├── serial.h        # Serial driver interface
│   ├── string.c        # String utility functions
│   ├── string.h        # String utility interface
│   ├── types.h         # Basic type definitions
│   ├── io.h            # I/O port operations
│   ├── link.ld         # Linker script
│   └── Makefile        # Build system
├── LICENSE   
├── Project_Report.pdf          # MIT License
└── README.md           # This file
```

## 🛠️ Build System

### Makefile Targets

| Command | Description |
|---------|-------------|
| `make` or `make all` | Build kernel.elf |
| `make run` | Run in QEMU (serial output only) |
| `make run-vga` | Run in QEMU (with VGA window) |
| `make debug` | Run in debug mode (GDB ready) |
| `make clean` | Remove build artifacts |

## 🧠 Core Components

### Memory Manager

The memory manager provides dynamic memory allocation and deallocation for the kernel.

**Files:** [memory.c](src/memory.c), [memory.h](src/memory.h)

**Key Features:**
- Dynamic memory allocation (`malloc`)
- Memory deallocation (`free`)
- Memory initialization and tracking
- Efficient heap management

**Example Usage:**
```c
#include "memory.h"

void *ptr = malloc(256);
if (ptr != NULL) {
    // Use allocated memory
    free(ptr);
}
```

### Process Manager

The process manager handles process creation, management, and termination.

**Files:** [process.c](src/process.c), [process.h](src/process.h)

**Key Features:**
- Create and initialize processes
- Manage process state and context
- Process termination and cleanup
- Process control block (PCB) management

**Process States:**
- `READY` - Process is ready to run
- `RUNNING` - Process is currently executing
- `BLOCKED` - Process is waiting for I/O or event
- `TERMINATED` - Process has completed

**Example Usage:**
```c
#include "process.h"

void my_process_function(void) {
    // Process code here
}

int pid = create_process(my_process_function, 5);  // Priority 5
```

### Scheduler

The scheduler implements priority-based process scheduling with context switching.

**Files:** [scheduler.c](src/scheduler.c), [scheduler.h](src/scheduler.h), [ctxsw.S](src/ctxsw.S)

**Key Features:**
- Priority-based scheduling algorithm
- Context switching between processes
- Voluntary yielding (`yield()`)
- Process queue management

**Scheduling Algorithm:**
- Processes are organized by priority (higher priority runs first)
- Within same priority, round-robin scheduling applies
- Processes can voluntarily yield control via `yield()`

**Example Usage:**
```c
#include "scheduler.h"

// Yield control to next ready process
yield();

// Initialize and run scheduler
schedule();
```

## 📚 Learning Resources

### Recommended Reading

- [XINU OS](https://xinu.cs.purdue.edu/) - Educational OS similar to kacchiOS
- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development guide
- [The Little OS Book](https://littleosbook.github.io/) - Practical OS development
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/) - OS concepts textbook

### Related Topics

- x86 Assembly Language
- Memory Management
- Process Scheduling
- System Calls
- Interrupt Handling

## 🤝 Contributing

Contributions are welcome! Whether you're fixing bugs, improving documentation, or extending features, please feel free to submit issues and pull requests.

### Guidelines

1. **Keep code educational** - Write clear, well-commented code that explains concepts
2. **Follow existing style** - Match the current code formatting and structure
3. **Document your changes** - Update relevant documentation and comments
4. **Test thoroughly** - Verify your changes work in QEMU before submitting
5. **Write descriptive commits** - Explain what and why, not just what

This project is licensed under the MIT License.

## 👨‍🏫 About

kacchiOS was created as an educational tool for learning operating system design and implementation. Rather than focusing solely on theory, it provides a complete, working OS kernel that demonstrates:

- **Real OS Concepts** - Process scheduling, memory management, and context switching
- **Hands-on Learning** - Readable, well-documented code suitable for OS courses
- **Extensible Design** - A solid foundation for exploring additional OS features
- **Practical Experience** - Students can understand how modern kernels actually work

The project successfully implements a functional priority-based scheduler, dynamic memory allocator, and multi-process execution environment in approximately 1000 lines of C and assembly code.

## 🙏 Acknowledgments

- Inspired by XINU OS
- Built with guidance from OSDev community
- Thanks to all students who have contributed
