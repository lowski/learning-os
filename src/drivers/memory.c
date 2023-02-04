#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "swi.h"
#include "../interrupts/scheduling.h"

#define write_sp(index) asm("MOV %%sp, %0" :: "r" (MEM_ADDR_SPECIAL_MODE_STACKS + (index + 1) * MEM_SIZE_SPECIAL_MODE_STACK));

#define MODE_MASK 0xFFFFFFE0
#define MAX_ALLOCATIONS 1024 // 0x400
#define MAX_ALLOCATION_SIZE 1024

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
    bit_t mmu_enable: 1;
    bit_t alignment_fault_enable: 1;
    bit_t dcache_enable: 1;
    // always set to 0b1111
    uint_t _reserved0: 4;
    enum cp15_cr_endianness endianness: 1;
    uint_t system_protection: 1;
    uint_t rom_protection: 1;
    uint_t: 2;
    uint_t icache_enable: 1;
    uint_t base_loc_exception_reg: 1;
    uint_t round_robin_replacement: 1;
    bit_t: 1;
    uint_t: 14;
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
    privileged_limited = 0b00,

    // privileged modes have RW
    user_none = 0b01,
    // privileged modes have RW
    user_read_only = 0b10,
    read_write = 0b11,
};

struct __attribute__((packed)) l1_tt_section_entry {
    // MUST be section
    enum l1_tt_entry_type type: 2;
    bit_t buffered: 1;
    bit_t cached: 1;

    // always set to 1
    bit_t reserved: 1;

    uint8_t domain: 4; // this can be set to 0 as we only use one domain
    bit_t: 1;
    enum l1_tt_access_control ap: 2;
    uint8_t: 8;

    // only 12 bits for the address prefix (0x0 - 0xfff; 0 - 4096)
    uint_t address: 12;
};

struct __attribute__((packed)) l1_tt_fine_entry {
    // MUST be fine
    enum l1_tt_entry_type type: 2;
    bit_t: 2;
    // MUST be 1
    bit_t reserved: 1;
    uint8_t domain: 4; // this can be set to 0 as we only use one domain
    bit_t: 3;

    // only 20 bits for the address prefix (0x0 - 0xfffff; 0 - 1048575)
    uint_t address: 20;
};

enum l2_tt_entry_type {
    l2_fault = 0b00,
    l2_large = 0b01,
    l2_small = 0b10,
    l2_tiny = 0b11,
};

struct __attribute__((packed)) l2_tt_tiny_entry {
    enum l2_tt_entry_type type: 2;
    bit_t buffered: 1;
    bit_t cached: 1;
    enum l1_tt_access_control ap: 2;
    uint_t: 4;

    // only the top 22 bits for the address prefix (0x0 - 0x40_0000)
    uint32_t address: 22;
};

struct page_tables {
    union master_page_entry {
        struct l1_tt_section_entry section_entry;
        struct l1_tt_fine_entry fine_entry;
    } l1[4096];

    struct l2_table {
        union l2_page_entry {
            struct l2_tt_tiny_entry tiny;
        } entries[4096];
    } l2_tables[16];
} *translation_table = (struct page_tables *)MEM_ADDR_MASTER_PAGE_TABLE;

struct allocation {
    uint32_t size;
    uint32_t pid;
    // the physical address of the allocation in memory
    ptr_t physical_ptr;
    // the logical address to use in the software
    ptr_t logical_ptr;
} *allocation_table = (struct allocation *)MEM_ADDR_ALLOCATION_TABLE;

//struct l1_tt_section_entry *translation_table = (struct l1_tt_section_entry*)MEM_ADDR_MASTER_PAGE_TABLE;

FUNC_PRIVILEGED
void write_mmu_dacr(struct cp15_domain_access_control_register value) {
    asm("LDR r0, [%0]" :: "r" (&value));
    asm("MCR p15, 0, r0, c3, c0, 0");
}

FUNC_PRIVILEGED
void write_mmu_cr(struct cp15_control_register value) {
    asm("LDR r0, [%0]" :: "r" (&value));
    asm("MCR p15, 0, r0, c1, c0, 0");
}

FUNC_PRIVILEGED
void write_mmu_ttb(void *ttb) {
    asm("MOV r0, %0" :: "r" (ttb));
    asm("MCR p15, 0, r0, c2, c0, 0");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

FUNC_PRIVILEGED
uint32_t mmu_get_fault_status() {
    asm("MRC p15, 0, r0, c5, c0, 0");
}

FUNC_PRIVILEGED
ptr_t mmu_get_fault_address() {
    asm("MRC p15, 0, r0, c6, c0, 0");
}

#pragma GCC diagnostic pop

FUNC_PRIVILEGED
void invalidate_tlb() {
    asm("MCR p15, 0, r0, c8, c5, 0");
    asm("MCR p15, 0, r0, c8, c6, 0");
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
inline struct l1_tt_section_entry *get_mpt_section(ptr_t addr) {
    return &(translation_table->l1[(uint32_t) addr >> 20].section_entry);
}

/**
 * Get the fine L2 table entry for the given address.
 *
 * @param addr the address to get the MPT entry for
 * @return the correct MPT entry
 */
FUNC_PRIVILEGED
inline struct l1_tt_fine_entry *get_mpt_fine(ptr_t addr) {
    return &(translation_table->l1[(uint32_t) addr >> 20].fine_entry);
}

/**
 * Get the tiny page entry from a fine L2 table for a given logical address.
 *
 * @param addr the logical address
 * @return the tiny page entry from the correct L2 table. NULL, if the address does not belong to a fine L2 table.
 */
FUNC_PRIVILEGED
inline struct l2_tt_tiny_entry *get_page_tiny(ptr_t addr) {
    struct l1_tt_fine_entry *entry = get_mpt_fine(addr);
    if (entry->type != fine) {
        // this means there is no fine table for the logical address
        return NULL;
    }
    struct l2_table *l2_table = (struct l2_table *)((uint32_t)entry->address << 12);
    uint32_t idx = (uint32_t) addr << 12 >> 22;
    return &l2_table->entries[idx].tiny;
}

/**
 * Write a section entry into the master page table (translation table).
 *
 * @param from_addr the address to remap from (onlu upper 12 bits are used)
 * @param to_addr the physical address to remap to (only upper 12 bits are used)
 * @param access the access for the section
 */
FUNC_PRIVILEGED
void section_remap(ptr_t from_addr, ptr_t to_addr, enum l1_tt_access_control access) {
    struct l1_tt_section_entry *entry = get_mpt_section(from_addr);
    entry->address = (uint32_t) to_addr >> 20;
    entry->ap = access;
    entry->reserved = 1;
    entry->type = section;
}

/**
 * Write a section entry into the master page table without any mapping.
 */
FUNC_PRIVILEGED
void set_section_access(ptr_t addr, enum l1_tt_access_control access) {
    section_remap(addr, addr, access);
}

/**
 * Set the type for a section.
 */
FUNC_PRIVILEGED
void set_section_type(ptr_t addr, enum l1_tt_entry_type type) {
    get_mpt_section(addr)->type = type;
}

FUNC_PRIVILEGED
void tiny_page_remap(ptr_t logical_addr, ptr_t physical_addr, enum l1_tt_access_control access) {
    struct l2_tt_tiny_entry *page = get_page_tiny(logical_addr);
    if (page == NULL) {
        // we can't remap a page that was not found.
        return;
    }
    page->address = (uint32_t) physical_addr >> 10;
    page->ap = access;
    page->type = l2_tiny;
}

/**
 * Set the access info on a tiny page.
 */
FUNC_PRIVILEGED
void set_tiny_page_access(ptr_t addr, enum l1_tt_access_control access) {
    tiny_page_remap(addr, addr, access);
}

/**
 * Setup a fine L2 page table. This includes 1:1 mapping all pages and setting
 * the access to privileged limited.
 *
 * @param logical_addr the logical address at which an L2 table should be created
 * @param table the table to use for the address (entries will be overridden).
 */
FUNC_PRIVILEGED
void setup_fine_table(ptr_t logical_addr, struct l2_table *table) {
    struct l1_tt_fine_entry *entry = get_mpt_fine(logical_addr);
    entry->type = fine;
    entry->reserved = 1;
    entry->address = (uint32_t) table >> 12;

    for (int i = 0; i < 4096; i++) {
        set_tiny_page_access(logical_addr + i * 4096, privileged_limited);
    }
}

FUNC_PRIVILEGED
void init_mmu() {
    // initialize sections by (1) setting 1:1 mapping, (2) forbidding user access, (3) setting section type to fault
    for (int i = 0; i < 4096; ++i) {
        ptr_t ptr = (ptr_t) (i << 20);
        set_section_access(ptr, privileged_limited);
        set_section_type(ptr, fault);
    }

    setup_fine_table(MEM_LADDR_HEAP_START, &translation_table->l2_tables[0]);

    // update the sections that actually should be readable

    section_remap(0x0, MEM_ADDR_SECTION_IVT, user_none);
    set_section_access(MEM_ADDR_MASTER_PAGE_TABLE, user_none);
    set_section_access(MEM_ADDR_SPECIAL_MODE_STACKS, user_none);
    set_section_access(MEM_ADDR_TCBS, user_none);
    set_section_access(MEM_ADDR_THREAD_STACKS, read_write);

    set_section_access(MEM_ADDR_SECTION_TEXT_PRIVILEGED, user_none);
    set_section_access(MEM_ADDR_SECTION_TEXT_USER, user_read_only);

    set_section_access(MEM_ADDR_PERIPHERY_AIC, user_none);
    set_section_access(MEM_ADDR_PERIPHERY_DBGU, user_none);
    set_section_access(MEM_ADDR_PERIPHERY_ST, user_none);

    // User mode RW access to all of these things is not great but needed until all the functions are implemented using
    // software interrupts.
    set_section_access(MEM_ADDR_SECTION_DATA_USER, read_write);
    set_section_access(MEM_ADDR_THREAD_STACKS, read_write);
    set_section_access(MEM_ADDR_TCBS, read_write);
    set_section_access(MEM_ADDR_PERIPHERY_DBGU, read_write);

    write_mmu_ttb(translation_table);

    struct cp15_domain_access_control_register mmu_da;
    mmu_da.d0 = client;
    write_mmu_dacr(mmu_da);

    struct cp15_control_register mmu_cr;
    mmu_cr.mmu_enable = 1;
    mmu_cr.dcache_enable = 0;
    mmu_cr.icache_enable = 0;
    mmu_cr.system_protection = 0;
    mmu_cr.rom_protection = 0;
    mmu_cr._reserved0 = 0b1111;
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
ptr_t malloc(__attribute__((unused)) uint32_t size) {
    raise_swi(SWI_CODE_MEM_MALLOC);
}
#pragma GCC diagnostic pop

void free(__attribute__((unused)) ptr_t ptr) {
    raise_swi(SWI_CODE_MEM_FREE);
}

FUNC_PRIVILEGED
ptr_t mem_malloc(uint32_t size) {
    if (size > MAX_ALLOCATION_SIZE) {
        printf("[EXCEPTION] malloc: size too large; must be <= MAX_ALLOCATION_SIZE.\n");
        return NULL;
    }
    struct allocation* alloc = NULL;
    for (int i = 0; i < MAX_ALLOCATIONS; ++i) {
        struct allocation *item = &allocation_table[i];
        if (item->pid == 0) {
            alloc = item;
            alloc->physical_ptr = MEM_PADDR_HEAP_START + i * MAX_ALLOCATION_SIZE;
            alloc->logical_ptr = MEM_LADDR_HEAP_START + i * MAX_ALLOCATION_SIZE;
            break;
        }
    }
    if (alloc == NULL) {
        printf("[EXCEPTION] malloc: out of memory.\n");
        return NULL;
    }
    alloc->size = size;
    alloc->logical_ptr += MAX_ALLOCATION_SIZE - size;
    alloc->physical_ptr += MAX_ALLOCATION_SIZE - size;
    alloc->pid = get_current_thread_id();

    tiny_page_remap(alloc->logical_ptr, alloc->physical_ptr, read_write);

    return alloc->logical_ptr;
}

FUNC_PRIVILEGED
void mem_free(ptr_t ptr) {
    for (int i = 0; i < MAX_ALLOCATIONS; ++i) {
        struct allocation *alloc = &allocation_table[i];
        if (alloc->logical_ptr == ptr) {
            alloc->pid = 0;
            set_tiny_page_access(alloc->logical_ptr, privileged_limited);
            invalidate_tlb();
            break;
        }
    }
}

FUNC_PRIVILEGED
void mem_swap_heap(uint32_t pid) {
    for (int i = 0; i < MAX_ALLOCATIONS; ++i) {
        struct allocation *alloc = &allocation_table[i];
        if (alloc->pid != pid) {
            set_tiny_page_access(alloc->logical_ptr , privileged_limited);
        } else {
            tiny_page_remap(alloc->logical_ptr, alloc->physical_ptr, read_write);
        }
    }
    invalidate_tlb();
}
