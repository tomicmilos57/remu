Remu - RISC-V RV32IMA_Zicsr Emulator
====================================

Overview
--------
Remu is a lightweight emulator for the RISC-V RV32IMA_Zicsr architecture, developed with the goal of running the xv6 operating system. It emulates a minimal RISC-V machine capable of executing real-world kernel code such as xv6.

Features
--------
- Full support for RV32I base integer instruction set
- RV32M standard extension for integer multiplication and division
- RV32A atomic extension
- Zicsr extension for control and status register (CSR) access
- Memory-mapped I/O interface for simple device emulation
- ELF loader for loading and executing xv6 kernel binaries
- Step-by-step execution and debugging support

Example
-------------
![2025-06-07-000313_402x933_scrot](https://github.com/user-attachments/assets/f90a9ee1-cc9a-4b35-96cd-5ea1909fb46f)
