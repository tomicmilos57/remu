#include "csr.h"


void CSR::set(uint32_t sel, uint32_t value) {
    switch (sel) {
        case 0xF14: mhartid = value; break;
        case 0x300: mstatus = value; break;
        case 0x341: mepc = value; break;
        case 0x100: sstatus = value; break;
        case 0x144: sip = value; break;
        case 0x104: sie = value; break;
        case 0x304: mie = value; break;
        case 0x141: sepc = value; break;
        case 0x302: medeleg = value; break;
        case 0x303: mideleg = value; break;
        case 0x105: stvec = value; break;
        case 0x305: mtvec = value; break;
        case 0x180: satp = value; break;
        case 0x142: scause = value; break;
        case 0x342: mcause = value; break;
        case 0x143: stval = value; break;
        case 0x306: mcounter = value; break;
        case 0xC01: time = value; break;
        case 0x140: sscratch = value; break;
        case 0x340: mscratch = value; break;
    }
}

uint32_t CSR::read(uint32_t sel) const {
    switch (sel) {
        case 0xF14: return mhartid;
        case 0x300: return mstatus;
        case 0x341: return mepc;
        case 0x100: return sstatus;
        case 0x144: return sip;
        case 0x104: return sie;
        case 0x304: return mie;
        case 0x141: return sepc;
        case 0x302: return medeleg;
        case 0x303: return mideleg;
        case 0x105: return stvec;
        case 0x305: return mtvec;
        case 0x180: return satp;
        case 0x142: return scause;
        case 0x342: return mcause;
        case 0x143: return stval;
        case 0x306: return mcounter;
        case 0xC01: return time;
        case 0x140: return sscratch;
        case 0x340: return mscratch;
        default: return 0;
    }
}
