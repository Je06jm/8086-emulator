# 6502-emulator
An 6502 emulator. The CPU is emulated, but no memory devices are provided by
default. Functions are available to add memory devices

## Adding custom modules
Modules allow for custom devices to be registered in the system. For example, 
a keyboard controller. What makes modules special is that they receive ticks
from the system.

### Module definition
```
typedef struct {
    E86_ModuleFunction_Init init;
    E86_ModuleFunction_Finish finish;
    E86_ModuleFunction_Tick tick;
    uint32_t reset;
    uint32_t counter;
} E86_Module;
```

The `init` function is called when the module is initialized. If this function 
is `NULL`, then initialization is skipped

The `finish` function is called when the module is cleaned up. If this
function is `NULL`, then cleanup is skipped

The `tick` function is called when the module's counter is zero. If this is
`NULL`, then ticking will be skipped

The `reset` variable is used to set the `counter` variable when it reaches
zero

The `counter` variable is decremented when the system ticks

### Module callback function signatures
```
typedef void (*E86_ModuleFunction_Init)();
typedef void (*E86_ModuleFunction_Finish)();
typedef void (*E86_ModuleFunction_Tick)();
```

### Registering modules
To register a module, call
```
void E65_ModuleAdd(E65_Module* module);
```
The `E65_ModuleAdd` copies the module definition so the original definition is
allowed to go out of scope

## Memory functions
Custom memory functions can be registered to allow the CPU to interact with
your custom modules. Functions are also available to read/write to memory

### Registering memory
```
void E65_MemoryRegisterReadByte(E65_MemoryFunction_ReadByte func);
void E65_MemoryRegisterWriteByte(E65_MemoryFunction_WriteByte func);
```

### Memory registration read/write function signatures
```
typedef E65_Byte (*E65_MemoryFunction_ReadByte)(E65_Word address);
typedef void (*E65_MemoryFunction_WriteByte)(E65_Word address, E65_Byte value);
```

### Memory read/write function
```
E65_Byte E65_MemoryReadByte(E65_Word address);
void E65_MemoryWriteByte(E65_Word address, E65_Byte value);
```

## Interrupts
There are two types are interrupts, non-maskable interrupt, called NMInterrupts 
in code, and interrupts. There are two functions used to set interrupts

### Interrupts
Interrupts are level triggered in real hardware so there is a single function
that can be used to set/reset the interrupt
```
void E65_InterruptSetInterrupt(bool interrupt);
```

### NMInterrupts
NMInterrupts are edge triggered in real hardware so there is a single function
to set the NMInterrupt
```
void E65_InterruptTriggerNMInterrupt();
```

## Example
A simple example is provided which calculates the fibonacci sequence up to
13. The result can be see by hex dumping OUT.bin

## Build
Scons is used as the build system

### Installing scons
```
python3 -m pip install scons
```

### Building
```
scons
```

To build with more than one thread, use `-jN`, where N is the number of threads.
To clean the build, use `-c`

### Additional flags
Two additional flags can be used `debug` and `example`. They can be used as so
```
scons debug=true example=true
```