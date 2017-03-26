// 1Kb of data
#define DATA_LENGTH   ((IFLASH1_PAGE_SIZE/sizeof(byte))*4)

// choose a start address that's offset to show that it doesn't have to be on a page boundary
#define  FLASH_START  ((byte *)IFLASH1_ADDR)

//  FLASH_DEBUG can be enabled to get debugging information displayed.
// #define FLASH_DEBUG

#define _FLASH_DEBUG(x)

/* Internal Flash 0 base address. */
#define IFLASH_ADDR     IFLASH0_ADDR
	/* Internal flash page size. */
#define IFLASH_PAGE_SIZE     IFLASH0_PAGE_SIZE

/* Last page start address. */
#define IFLASH_LAST_PAGE_ADDRESS (IFLASH1_ADDR + IFLASH1_SIZE - IFLASH1_PAGE_SIZE)

#define FLASH_ACCESS_MODE_128    EFC_ACCESS_MODE_128
#define FLASH_ACCESS_MODE_64     EFC_ACCESS_MODE_64


#define EFC_FCMD_GETD    0x00  //!< Get Flash Descriptor
#define EFC_FCMD_WP      0x01  //!< Write page
#define EFC_FCMD_WPL     0x02  //!< Write page and lock
#define EFC_FCMD_EWP     0x03  //!< Erase page and write page
#define EFC_FCMD_EWPL    0x04  //!< Erase page and write page then lock
#define EFC_FCMD_EA      0x05  //!< Erase all
#define EFC_FCMD_SLB     0x08  //!< Set Lock Bit
#define EFC_FCMD_CLB     0x09  //!< Clear Lock Bit
#define EFC_FCMD_GLB     0x0A  //!< Get Lock Bit
#define EFC_FCMD_SGPB    0x0B  //!< Set GPNVM Bit
#define EFC_FCMD_CGPB    0x0C  //!< Clear GPNVM Bit
#define EFC_FCMD_GGPB    0x0D  //!< Get GPNVM Bit
#define EFC_FCMD_STUI    0x0E  //!< Start unique ID
#define EFC_FCMD_SPUI    0x0F  //!< Stop unique ID

#define CHIP_FLASH_IAP_ADDRESS  (IROM_ADDR + 8)

#define EFC_ACCESS_MODE_128    0
#define EFC_ACCESS_MODE_64     EEFC_FMR_FAM

typedef enum flash_rc {
	FLASH_RC_OK = 0,        //!< Operation OK
	FLASH_RC_YES = 0,       //!< Yes
	FLASH_RC_NO = 1,        //!< No
	FLASH_RC_ERROR = 0x10,  //!< General error
	FLASH_RC_INVALID,       //!< Invalid argument input
	FLASH_RC_NOT_SUPPORT = 0xFFFFFFFF    //!< Operation is not supported
} flash_rc_t;

typedef enum flash_farg_page_num {
	/* 4 of pages to be erased with EPA command*/
	IFLASH_ERASE_PAGES_4=0,
	/* 8 of pages to be erased with EPA command*/
	IFLASH_ERASE_PAGES_8,
	/* 16 of pages to be erased with EPA command*/
	IFLASH_ERASE_PAGES_16,
	/* 32 of pages to be erased with EPA command*/
	IFLASH_ERASE_PAGES_32,
	/* Parameter is not support */
	IFLASH_ERASE_PAGES_INVALID,
}flash_farg_page_num_t;

static uint32_t gs_ul_page_buffer[IFLASH_PAGE_SIZE / sizeof(uint32_t)];
uint32_t retCode;

inline void DueFlashStorage() {
  /* Initialize flash: 6 wait states for flash writing. */
  retCode = flash_init(FLASH_ACCESS_MODE_128, 4);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Flash init failed\n");
  }
}

inline byte read(uint32_t address) {
  return FLASH_START[address];
}

inline byte* readAddress(uint32_t address) {
  return FLASH_START+address;
}

inline boolean write(uint32_t address, byte value) {
  uint32_t retCode;
  uint32_t byteLength = 1;  
  byte *data;

  retCode = flash_unlock((uint32_t)FLASH_START+address, (uint32_t)FLASH_START+address + byteLength - 1, 0, 0);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Failed to unlock flash for write\n");
    return false;
  }

  // write data
  retCode = flash_write((uint32_t)FLASH_START+address, &value, byteLength, 1);
  //retCode = flash_write((uint32_t)FLASH_START, data, byteLength, 1);

  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Flash write failed\n");
    return false;
  }

  // Lock page
  retCode = flash_lock((uint32_t)FLASH_START+address, (uint32_t)FLASH_START+address + byteLength - 1, 0, 0);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Failed to lock flash page\n");
    return false;
  }
  return true;
}


inline void write_noblocking(uint32_t address, byte value) {
  uint32_t retCode;
  uint32_t byteLength = 1;  
  byte *data;

  retCode = flash_unlock((uint32_t)FLASH_START+address, (uint32_t)FLASH_START+address + byteLength - 1, 0, 0);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Failed to unlock flash for write\n");
    //return false;
  }

  // write data
  flash_write_noblocking((uint32_t)FLASH_START+address, &value, 1, 1);

}

inline bool write_end(uint32_t address) {
  
  //flash_lock_bit = 0;
  
  // efc_set_wait_state(p_efc, ul_fws_temp);
  efc_set_wait_state(EFC1, 4);

/*  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Flash write failed\n");
    return false;
  }
*/

  // Lock page
  retCode = flash_lock((uint32_t)FLASH_START+address, (uint32_t)FLASH_START+address + 1 - 1, 0, 0);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Failed to lock flash page\n");
    return false;
  }
  return true;
}


inline boolean write(uint32_t address, byte *data, uint32_t dataLength) {
  uint32_t retCode;

  if ((uint32_t)FLASH_START+address < IFLASH1_ADDR) {
    _FLASH_DEBUG("Flash write address too low\n");
    return false;
  }

  if ((uint32_t)FLASH_START+address >= (IFLASH1_ADDR + IFLASH1_SIZE)) {
    _FLASH_DEBUG("Flash write address too high\n");
    return false;
  }

  if (((uint32_t)FLASH_START+address & 3) != 0) {
    _FLASH_DEBUG("Flash start address must be on four byte boundary\n");
    return false;
  }

  // Unlock page
  retCode = flash_unlock((uint32_t)FLASH_START+address, (uint32_t)FLASH_START+address + dataLength - 1, 0, 0);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Failed to unlock flash for write\n");
    return false;
  }

  // write data
  retCode = flash_write((uint32_t)FLASH_START+address, data, dataLength, 1);

  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Flash write failed\n");
    return false;
  }

  // Lock page
    retCode = flash_lock((uint32_t)FLASH_START+address, (uint32_t)FLASH_START+address + dataLength - 1, 0, 0);
  if (retCode != FLASH_RC_OK) {
    _FLASH_DEBUG("Failed to lock flash page\n");
    return false;
  }
  return true;
}

