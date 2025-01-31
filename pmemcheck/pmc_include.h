/*
 * Persistent memory checker.
 * Copyright (c) 2015-2020, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, or (at your option) any later version, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef PMC_INCLUDE_H
#define PMC_INCLUDE_H
/*
 * Metadata structure for store information array.
 * Start index and end index point to elements in pmem_stores.
 * The metadata stores information (min_addr, max_addr and flushed_state) for 
 * elements between start index and end index */
struct arr_md {

    UWord start_index; 
    UWord end_index; 
    Addr min_addr;
    Addr max_addr;
    enum flushed_state {
        NO_FLUSHED,
        PART_FLUSHED,
        ALL_FLUSHED,
    } state;
};


/** Single store to memory. */
struct pmem_st {
    Addr addr;
    ULong size;
    ULong block_num;
    UWord value;
    ExeContext *context;
    enum store_state {
        STST_CLEAN,
        STST_DIRTY,
        STST_FLUSHED,
    } state;
};

/*------------------------------------------------------------*/
/*--- Common functions                                     ---*/
/*------------------------------------------------------------*/
/* Check if the given region is in the set. */
UWord is_in_mapping_set(const struct pmem_st *region, OSet *region_set);

/* Add a region to a set. */
void add_region(const struct pmem_st *region, OSet *region_set);

/* Remove a region from a set. */
void remove_region(const struct pmem_st *region, OSet *region_set);

/**
* \brief A compare function for regions stored in the OSetGen.
* \param[in] key The key to compare.
* \param[in] elem The element to compare with.
* \return -1 if key is smaller, 1 if key is greater and 0 if key is equal
* to elem. This means that the region is either before, after or overlaps.
*/
static inline __attribute__((always_inline)) Word
cmp_pmem_st(const void *key, const void *elem)
{
    const struct pmem_st *lhs = (const struct pmem_st *) (key);
    const struct pmem_st *rhs = (const struct pmem_st *) (elem);

    if (lhs->addr + lhs->size <= rhs->addr)
        return -1;
    else if (lhs->addr >= rhs->addr + rhs->size)
        return 1;
    else
        return 0;
}

/* Check and update the given warning event register. */
void add_warning_event(struct pmem_st **event_register, UWord *nevents,
                  struct pmem_st *event, UWord limit, void (*err_msg)(UWord));

/* Check if regions overlap */
UWord check_overlap(const struct pmem_st *lhs, const struct pmem_st *rhs);

/*------------------------------------------------------------*/
/*--- Transactions related                                 ---*/
/*------------------------------------------------------------*/

/* Initialize the transactions module */
void init_transactions(Bool transactions_only);

/* Add a new transaction */
void register_new_tx(UWord tx_id);

/* Remove a transaction */
UInt remove_tx(UWord tx_id);

/* Add a memory region to a transaction */
UInt add_obj_to_tx(UWord tx_id, UWord base, UWord size);

/* Remove a registered region from the given transaction */
UInt remove_obj_from_tx(UWord tx_id, UWord base, UWord size);

/* Explicitely add a thread to a transaction */
UInt add_thread_to_tx(UWord tx_id);

/* Explicitely remove a thread from a transaction */
UInt remove_thread_from_tx(UWord tx_id);

/* Exclude a memory region from analysis */
void add_to_global_excludes(const struct pmem_st *region);

/* Handle the store made to PMEM in regards to running transactions */
void handle_tx_store(const struct pmem_st *store);

/* Print the summary of transaction analysis */
void print_tx_summary(void);

/* Return all errors referenced to transactions */
UWord get_tx_all_err(void);

#endif	/* PMC_INCLUDE_H */

