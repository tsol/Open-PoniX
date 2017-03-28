/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 2011 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#ifndef _NV_P2P_H_
#define _NV_P2P_H_

enum {
    NVIDIA_P2P_ARCHITECTURE_TESLA = 0,
    NVIDIA_P2P_ARCHITECTURE_FERMI,
    NVIDIA_P2P_ARCHITECTURE_CURRENT = NVIDIA_P2P_ARCHITECTURE_FERMI
};

#define NVIDIA_P2P_PARAMS_VERSION   0x00010001

enum {
    NVIDIA_P2P_PARAMS_ADDRESS_INDEX_GPU = 0,
    NVIDIA_P2P_PARAMS_ADDRESS_INDEX_THIRD_PARTY_DEVICE,
    NVIDIA_P2P_PARAMS_ADDRESS_INDEX_MAX = \
        NVIDIA_P2P_PARAMS_ADDRESS_INDEX_THIRD_PARTY_DEVICE
};

typedef
struct nvidia_p2p_params {
    uint32_t version;
    uint32_t architecture;
    union nvidia_p2p_mailbox_addresses {
        struct {
            uint64_t wmb_addr;
            uint64_t wmb_data;
            uint64_t rreq_addr;
            uint64_t rcomp_addr;
            uint64_t reserved[2];
        } fermi;
    } addresses[NVIDIA_P2P_PARAMS_ADDRESS_INDEX_MAX+1];
} nvidia_p2p_params_t;

int nvidia_p2p_init_mapping(uint64_t p2p_token,
        struct nvidia_p2p_params *params,
        void (*destroy_callback)(void *data),
        void *data);
int nvidia_p2p_destroy_mapping(uint64_t p2p_token);

enum {
    NVIDIA_P2P_PAGE_SIZE_4KB = 0,
    NVIDIA_P2P_PAGE_SIZE_64KB,
    NVIDIA_P2P_PAGE_SIZE_128KB
};

typedef
struct nvidia_p2p_page {
    uint64_t physical_address;
    union nvidia_p2p_request_registers {
        struct {
            uint32_t wreqmb_h;
            uint32_t rreqmb_h;
            uint32_t rreqmb_0;
            uint32_t reserved[3];
        } fermi;
    } registers;
} nvidia_p2p_page_t;

#define NVIDIA_P2P_PAGE_TABLE_VERSION   0x00010001

typedef
struct nvidia_p2p_page_table {
    uint32_t version;
    uint32_t page_size;
    struct nvidia_p2p_page **pages;
    uint32_t entries;
} nvidia_p2p_page_table_t;

int nvidia_p2p_get_pages(uint64_t p2p_token, uint32_t va_space,
        uint64_t virtual_address,
        uint64_t length,
        struct nvidia_p2p_page_table **page_table,
        void (*free_callback)(void *data),
        void *data);
int nvidia_p2p_put_pages(uint64_t p2p_token, uint32_t va_space,
        uint64_t virtual_address,
        struct nvidia_p2p_page_table *page_table);

int nvidia_p2p_free_page_table(struct nvidia_p2p_page_table *page_table);

#endif /* _NV_P2P_H_ */
