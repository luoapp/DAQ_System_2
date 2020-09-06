/* daqdrv

   daq card driver

   Copyright 2007, Mitchell C. Nelson, Ph.D.

   No representation is provided of suitability for any purpose whatsoever.

*/

#ifndef _DAQDRVMAIN_H_
#define _DAQDRVMAIN_H_

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#include <asm/uaccess.h>

#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/sched.h>

#define DAQDRV_SUBSYSTEM_VENDOR_GSC 0x10b5
//#define DAQDRV_SUBSYSTEM_ID_16AISS8AO4 0x3172
//#define DAQDRV_SUBSYSTEM_ID_16AISS16AO2 0x3243

/* debugging option
 */
//#define DAQDRV_DEBUG

#ifdef DAQDRV_DEBUG
#define DEBUGMSG(fmt, args...) printk( KERN_NOTICE "daqdrv: " fmt, ## args )
#else
#define DEBUGMSG(fmt, args...)
#endif

#define DAQDRVMSG(fmt, args...) printk( KERN_NOTICE "daqdrv: " fmt, ## args )

/* this is used for dma to/from kernel memory
 */
typedef struct daqdrv_dmamem {
  void       *vaddr;
  size_t     size;
  dma_addr_t paddr;

  // for buffer pingponging;
  unsigned int count;

} DAQDRV_DmaMem;

/* this is the scatter gather list
 */
typedef struct daqdrv_dmaseg {
  dma_addr_t   addr;
  u32 len;        
  void *next;     // the plx chip uses 4 bits in the pointer
  u32 controls;   // controls
} DAQDRV_DmaSeg;

#define SEGINTBIT 0x1  // this is our interrupt bit for now.
#define SEGID     0xFAB0
#define SEGIDMASK 0xFFF0

/* this manages the user pages
 */
typedef struct daqdrv_dmauser {

  struct page **pages;
  int npages;

  struct scatterlist *sg;
  int nsg;

  DAQDRV_DmaSeg    *seg;
  int nseg;

  unsigned long uaddr;
  int           usize;

  void *next;

} DAQDRV_DmaUser;

/* this is the total management for one dma
 */
typedef struct daqdrv_dmaengine {

  DAQDRV_DmaUser   *userhead;
  DAQDRV_DmaUser   *usertail;
  DAQDRV_DmaUser   *user;
  
  DAQDRV_DmaSeg    *seghead;
  DAQDRV_DmaSeg    *segtail;
  DAQDRV_DmaSeg    *seg;

  unsigned int     counter;
  unsigned int     enabled;
  unsigned int     overflow;

  unsigned long    uaddr;
  unsigned int     usize;

} DAQDRV_Dma;


/* this is device
 */

typedef struct daqdrv_dev {

  struct pci_dev  *pcidev;

  u16             subvendor;
  u16             subid;

  // local address space for the board functions
  unsigned long    phybase;
  void            *iobase;

  // board specific functions
  void             (*readcontrols)( void *, void * );
  void             (*writecontrols)( void *, void * );
  void             (*triggerinput)( void *);
  void             (*triggeroutput)( void *);
  void             (*triggerinputoutput)( void *);

  // local registers for the plx chip
  unsigned long    plxphy;
  void            *plxbase;

  // interrupt handler
  unsigned int     irq;
  int             (*irqhandler)(void *, void *);
  void            *irqarg;

  int              status;

  // status at interrupt
  int              intstatus;
  int              plxstatus;

  // time at interrupt
  unsigned long      jiffie;
  unsigned long long cycle;

  struct cdev      cdev;
  struct semaphore sem;

  DAQDRV_DmaMem    dmamem;

  DAQDRV_Dma       dma0;
  DAQDRV_Dma       dma1;

  struct fasync_struct    *async_queue;

  unsigned long     userdata;

  // interrupt spin lock
  spinlock_t       lock;

} DAQDRV_Dev;

void daqdrv_dma_reset( DAQDRV_Dev *daqdev, int channel );
DAQDRV_DmaSeg *daqdrv_dma_get_( DAQDRV_DmaUser *dma );

#endif
