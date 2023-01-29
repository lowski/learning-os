#include "memory.h"
#include "../stdlib/datatypes.h"
#include "../stdlib/stdio.h"

#define MEMORY_CONTROLLER 0xFFFFFF00
static volatile unsigned int * const memory_controller = (unsigned int *)MEMORY_CONTROLLER;

#define write_sp(index) asm("MOV %%sp, %0" :: "r" (MEM_ADDR_SPECIAL_MODE_STACKS + (index + 1) * MEM_SIZE_SPECIAL_MODE_STACK));

#define MODE_MASK 0xFFFFFFE0

enum cp15_cr_endianness {
    little,
    big,
};
enum cp15_cr_clocking_mode {
    fast_bus,
    synchronous,
    asynchronous = 0b11,
};
enum mmu_domain_access {
    no_access,
    client,
    manager = 0b11,
};

struct cp15_control_register {
    uint_t mmu_enable: 1;
    uint_t alignment_fault_enable: 1;
    uint_t dcache_enable: 1;
    uint_t reserved0: 4;
    enum cp15_cr_endianness endianness: 1;
    uint_t system_protection: 1;
    uint_t rom_protection: 1;
    uint_t reserved1: 2;
    uint_t icache_enable: 1;
    uint_t base_loc_exception_reg: 1;
    uint_t round_robin_replacement: 1;
    uint_t reserved2: 1;
    char reserved3;
    uint_t reserved4: 6;
    enum cp15_cr_clocking_mode clocking_mode: 2;
};

struct cp15_domain_access_control_register {
    enum mmu_domain_access d0: 2;
    enum mmu_domain_access d1: 2;
    enum mmu_domain_access d2: 2;
    enum mmu_domain_access d3: 2;
    enum mmu_domain_access d4: 2;
    enum mmu_domain_access d5: 2;
    enum mmu_domain_access d6: 2;
    enum mmu_domain_access d7: 2;
    enum mmu_domain_access d8: 2;
    enum mmu_domain_access d9: 2;
    enum mmu_domain_access d10: 2;
    enum mmu_domain_access d11: 2;
    enum mmu_domain_access d12: 2;
    enum mmu_domain_access d13: 2;
    enum mmu_domain_access d14: 2;
    enum mmu_domain_access d15: 2;
};

enum l1_tt_entry_type {
    fault,
    coarse,
    section,
    fine,
};

enum l1_tt_access_control {
    // privileged/user modes have RO or none, depending on SR bits in CTRL registers
    // S: 0, R: 0 ; none (permission fault)
    // S: 0, R: 1 ; both RO
    // S: 1, R: 0 ; privileged RO
    read_only,

    // privileged modes have RW
    user_none,
    // privileged modes have RW
    user_read_only,
    read_write,
};

struct __attribute__((packed)) l1_tt_section_entry {
    enum l1_tt_entry_type type: 2;
    bit_t buffered: 1;
    bit_t cached: 1;
    bit_t reserved0: 1; // set to 1
    uint8_t domain: 4; // this can be set to 0 as we only use one domain
    bit_t reserved1: 1;
//    bit_t ap: 2;
    enum l1_tt_access_control ap: 2;
    uint8_t reserved2: 8;

    // only 12 bits for the address prefix (0x0 - 0xfff; 0 - 4096)
    uint_t address: 12;
};

struct l1_tt_section_entry *translation_table = (struct l1_tt_section_entry*)MEM_ADDR_MASTER_PAGE_TABLE;

FUNC_PRIVILEGED
void write_mmu_dacr(struct cp15_domain_access_control_register value) {
    asm("MOV r0, %0" :: "r" (value));
    asm("MCR p15, 0, r0, c3, c0, 0");
}

FUNC_PRIVILEGED
void write_mmu_cr(struct cp15_control_register value) {
    asm("MOV r0, %0" :: "r" (value));
    asm("MCR p15, 0, r0, c1, c0, 0");
}

FUNC_PRIVILEGED
void write_mmu_ttb(void *ttb) {
    asm("MOV r0, %0" :: "r" (ttb));
    asm("MCR p15, 0, r0, c2, c0, 0");
}

unsigned int get_current_mode() {
    register int cpsr;
    asm("MRS %0, CPSR" : "=r" (cpsr));
    return cpsr & (~MODE_MASK);
}

/**
 * Get the section entry for the given address.
 *
 * @param addr the address to get the MPT entry for
 * @return the correct MPT entry
 */
FUNC_PRIVILEGED
inline struct l1_tt_section_entry *get_mpt_section(uint32_t addr) {
    return &translation_table[addr >> 20];
}

/**
 * Write a section entry into the master page table (translation table).
 *
 * @param from_addr the address to remap from (onlu upper 12 bits are used)
 * @param to_addr the physical address to remap to (only upper 12 bits are used)
 * @param access the access for the section
 */
FUNC_PRIVILEGED
void section_remap(uint32_t from_addr, uint32_t to_addr, enum l1_tt_access_control access) {
    struct l1_tt_section_entry *entry = get_mpt_section(from_addr);
    entry->address = to_addr >> 20;
    entry->ap = access;
    entry->reserved0 = 1;
    entry->type = section;
}

/**
 * Write a section entry into the master page table without any mapping.
 */
FUNC_PRIVILEGED
void set_section_access(uint32_t addr, enum l1_tt_access_control access) {
    section_remap(addr, addr, access);
}

FUNC_PRIVILEGED
void init_mmu() {
    // initialize sections by (1) setting 1:1 mapping, (2) forbidding user access, (3) setting section type to fault
    for (int i = 0; i < 4096; ++i) {
        set_section_access(i << 20, user_none);
        get_mpt_section(i << 20)->type = fault;
    }

    // update the sections that actually should be readable

    section_remap(0x0, MEM_ADDR_SECTION_IVT, user_none);
    set_section_access(MEM_ADDR_MASTER_PAGE_TABLE, user_none);
    set_section_access(MEM_ADDR_SPECIAL_MODE_STACKS, user_none);
    set_section_access(MEM_ADDR_TCBS, user_none);
    set_section_access(MEM_ADDR_THREAD_STACKS, read_write);

    set_section_access(MEM_ADDR_SECTION_TEXT_PRIVILEGED, read_only);
    set_section_access(MEM_ADDR_SECTION_TEXT_USER, read_only);

    set_section_access(MEM_ADDR_PERIPHERY_AIC, user_none);
    set_section_access(MEM_ADDR_PERIPHERY_DBGU, user_none);
    set_section_access(MEM_ADDR_PERIPHERY_ST, user_none);

    // User mode RW access to all of these things is not great but needed until all the functions are implemented using
    // software interrupts.
    set_section_access(MEM_ADDR_SECTION_TEXT_USER, user_read_only);
    set_section_access(MEM_ADDR_SECTION_DATA_USER, read_write);
    set_section_access(MEM_ADDR_THREAD_STACKS, read_write);
    set_section_access(MEM_ADDR_TCBS, read_write);
    set_section_access(MEM_ADDR_PERIPHERY_DBGU, read_write);

    write_mmu_ttb((void *) MEM_ADDR_MASTER_PAGE_TABLE);

    struct cp15_domain_access_control_register mmu_da;
    mmu_da.d0 = client;
    write_mmu_dacr(mmu_da);

    struct cp15_control_register mmu_cr;
    mmu_cr.mmu_enable = 1;
    mmu_cr.dcache_enable = 0;
    mmu_cr.icache_enable = 0;
    mmu_cr.system_protection = 1;
    mmu_cr.rom_protection = 0;
    mmu_cr.reserved0 = 0b1111;
    write_mmu_cr(mmu_cr);
}

FUNC_PRIVILEGED
void init_memory() {
    // Initialize stack pointers
    switch_mode(MODE_FIQ);
    write_sp(0);
    switch_mode(MODE_IRQ);
    write_sp(1);
    switch_mode(MODE_SVC);
    write_sp(2);
    switch_mode(MODE_ABT);
    write_sp(3);
    switch_mode(MODE_UND);
    write_sp(4);
    switch_mode(MODE_SYS);
    write_sp(5);

    // user mode is ignored for now, as we won't get out anymore :/

    // Go to user mode and initialize stack pointer
    switch_mode(MODE_SVC);

    init_mmu();
}

void demo_mmu() {
    printf("Accessing null pointer...\n\n");
    asm("MOV r0, #0");
    asm("LDR r0, [r0]");

    printf("\nReading init_memory function (kernel code)...\n");
    printf("first instruction of init_memory: %x\n", *(uint32_t *)init_memory);

    printf("\nThere is no kernel data, as we unfortunately need everything in the user space (see memory.c:184).\n");

    printf("\nWriting demo_mmu code...\n");
    *(uint32_t *)demo_mmu = 0xdeadbeef;

    printf("\nWe can't produce a stack overflow, as the stacks are all after one-another and currently not separated by any memory that could produce a fault.\n");

    printf("\nReading non-existent address (0xefff0000)...\n");
    printf("Value: %x\n", *(uint32_t *)0xefff0000);

    printf("\nThe IVT is mapped through the translation table, which demonstrates non 1:1 mapping (see memory.c:171)\n");
}
