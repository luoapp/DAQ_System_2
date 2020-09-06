
#ifndef _PLX_REGS_H_
#define _PLX_REGS_H_

#ifndef PCI_DEVICE_ID_PLX_9056
#define PCI_DEVICE_ID_PLX_9056 0x9056
#endif

//** DMA Registers **
#ifndef DMA_CH_0_MODE
#define DMA_CH_0_MODE				(0x80)
#endif
#ifndef DMA_CH_0_PCI_ADDR
#define DMA_CH_0_PCI_ADDR			(0x84)
#endif
#ifndef DMA_CH_0_LOCAL_ADDR
#define DMA_CH_0_LOCAL_ADDR			(0x88)
#endif
#ifndef DMA_CH_0_TRANS_BYTE_CNT
#define DMA_CH_0_TRANS_BYTE_CNT		(0x8C)
#endif
#ifndef DMA_CH_0_DESC_PTR
#define DMA_CH_0_DESC_PTR			(0x90)
#endif

#ifndef DMA_CH_1_MODE
#define DMA_CH_1_MODE				(0x94)
#endif
#ifndef DMA_CH_1_PCI_ADDR
#define DMA_CH_1_PCI_ADDR			(0x98)
#endif
#ifndef DMA_CH_1_LOCAL_ADDR
#define DMA_CH_1_LOCAL_ADDR			(0x9C)
#endif
#ifndef DMA_CH_1_TRANS_BYTE_CNT
#define DMA_CH_1_TRANS_BYTE_CNT		(0xA0)
#endif
#ifndef DMA_CH_1_DESC_PTR
#define DMA_CH_1_DESC_PTR			(0xA4)
#endif

#ifndef DMA_CMD_STATUS
#define DMA_CMD_STATUS				(0xA8)
#endif

// these two are byte wide. 
#ifndef DMA_CMD_STATUS_0
#define DMA_CMD_STATUS_0			(0xA8)
#endif

#ifndef DMA_CMD_STATUS_1
#define DMA_CMD_STATUS_1			(0xA9)
#endif

#ifndef DMA_MODE_ARBITRATION
#define DMA_MODE_ARBITRATION		(0xAC)
#endif

#ifndef DMA_THRESHOLD_REG
#define DMA_THRESHOLD_REG			(0xB0)
#endif

#ifndef DMA_DAC0
#define DMA_DAC0                                (0xB4)
#endif

#ifndef DMA_DAC1
#define DMA_DAC1                                (0xB8)
#endif

//** Local Configuration Registers. **
#ifndef PCI_TO_LOC_ADDR_0_RNG
#define PCI_TO_LOC_ADDR_0_RNG		(0x00)
#endif
#ifndef LOC_BASE_ADDR_REMAP_0
#define LOC_BASE_ADDR_REMAP_0		(0x04)
#endif
#ifndef MODE_ARBITRATION
#define MODE_ARBITRATION			(0x08)
#endif
#ifndef BIG_LITTLE_ENDIAN_DESC
#define BIG_LITTLE_ENDIAN_DESC		(0x0C)
#endif
#ifndef PCI_TO_LOC_ROM_RNG
#define PCI_TO_LOC_ROM_RNG			(0x10)
#endif
#ifndef LOC_BASE_ADDR_REMAP_EXP_ROM
#define LOC_BASE_ADDR_REMAP_EXP_ROM	(0x14)
#endif
#ifndef BUS_REG_DESC_0_FOR_PCI_LOC
#define BUS_REG_DESC_0_FOR_PCI_LOC	(0x18)
#endif
#ifndef DIR_MASTER_TO_PCI_RNG
#define DIR_MASTER_TO_PCI_RNG		(0x1C)
#endif
#ifndef LOC_ADDR_FOR_DIR_MASTER_MEM
#define LOC_ADDR_FOR_DIR_MASTER_MEM	(0x20)
#endif
#ifndef LOC_ADDR_FOR_DIR_MASTER_IO
#define LOC_ADDR_FOR_DIR_MASTER_IO	(0x24)
#endif
#ifndef PCI_ADDR_REMAP_DIR_MASTER
#define PCI_ADDR_REMAP_DIR_MASTER	(0x28)
#endif
#ifndef PCI_CFG_ADDR_DIR_MASTER_IO
#define PCI_CFG_ADDR_DIR_MASTER_IO	(0x2C)
#endif


#ifndef PCI_TO_LOC_ADDR_1_RNG
#define PCI_TO_LOC_ADDR_1_RNG		(0xF0)
#endif
#ifndef LOC_BASE_ADDR_REMAP_1
#define LOC_BASE_ADDR_REMAP_1		(0xF4)
#endif
#ifndef BUS_REG_DESC_1_FOR_PCI_LOC
#define BUS_REG_DESC_1_FOR_PCI_LOC	(0xF8)
#endif


//** Run Time Registers **
#ifndef MAILBOX_REGISTER_0
#define MAILBOX_REGISTER_0			(0x40)
#endif
#ifndef MAILBOX_REGISTER_1
#define MAILBOX_REGISTER_1			(0x44)
#endif
#ifndef MAILBOX_REGISTER_2
#define MAILBOX_REGISTER_2			(0x48)
#endif
#ifndef MAILBOX_REGISTER_3
#define MAILBOX_REGISTER_3			(0x4C)
#endif
#ifndef MAILBOX_REGISTER_4
#define MAILBOX_REGISTER_4			(0x50)
#endif
#ifndef MAILBOX_REGISTER_5
#define MAILBOX_REGISTER_5			(0x54)
#endif
#ifndef MAILBOX_REGISTER_6
#define MAILBOX_REGISTER_6			(0x58)
#endif
#ifndef MAILBOX_REGISTER_7
#define MAILBOX_REGISTER_7			(0x5C)
#endif
#ifndef PCI_TO_LOC_DOORBELL
#define PCI_TO_LOC_DOORBELL			(0x60)
#endif
#ifndef LOC_TO_PCI_DOORBELL
#define LOC_TO_PCI_DOORBELL			(0x64)
#endif
#ifndef INT_CTRL_STATUS
#define INT_CTRL_STATUS				(0x68)
#endif
#ifndef PROM_CTRL_CMD_CODES_CTRL
#define PROM_CTRL_CMD_CODES_CTRL	(0x6C)
#endif
#ifndef DEVICE_ID_VENDOR_ID
#define DEVICE_ID_VENDOR_ID			(0x70)
#endif
#ifndef REVISION_ID
#define REVISION_ID					(0x74)
#endif
#ifndef MAILBOX_REG_0
#define MAILBOX_REG_0				(0x78)
#endif
#ifndef MAILBOX_REG_1
#define MAILBOX_REG_1				(0x7C)
#endif

//** Messaging Queue Registers **
#ifndef OUT_POST_Q_INT_STATUS
#define OUT_POST_Q_INT_STATUS        (0x30)
#endif
#ifndef OUT_POST_Q_INT_MASK
#define OUT_POST_Q_INT_MASK          (0x34)
#endif
#ifndef IN_Q_PORT
#define IN_Q_PORT                    (0x40)
#endif
#ifndef OUT_Q_PORT
#define OUT_Q_PORT                   (0x44)
#endif

#ifndef MSG_UNIT_CONFIG
#define MSG_UNIT_CONFIG              (0xC0)
#endif
#ifndef Q_BASE_ADDR
#define Q_BASE_ADDR                  (0xC4)
#endif
#ifndef IN_FREE_HEAD_PTR
#define IN_FREE_HEAD_PTR             (0xC8)
#endif
#ifndef IN_FREE_TAIL_PTR
#define IN_FREE_TAIL_PTR             (0xCC)
#endif
#ifndef IN_POST_HEAD_PTR
#define IN_POST_HEAD_PTR             (0xD0)
#endif
#ifndef IN_POST_TAIL_PTR
#define IN_POST_TAIL_PTR             (0xD4)
#endif
#ifndef OUT_FREE_HEAD_PTR
#define OUT_FREE_HEAD_PTR            (0xD8)
#endif
#ifndef OUT_FREE_TAIL_PTR
#define OUT_FREE_TAIL_PTR            (0xDC)
#endif
#ifndef OUT_POST_HEAD_PTR
#define OUT_POST_HEAD_PTR            (0xE0)
#endif
#ifndef OUT_POST_TAIL_PTR
#define OUT_POST_TAIL_PTR            (0xE4)
#endif
#ifndef Q_STATUS_CTRL_REG
#define Q_STATUS_CTRL_REG            (0xE8)
#endif

// Interrupt Control/Status register masks

#ifndef IRQ_LOCAL_LSERR_ABORT
#define IRQ_LOCAL_LSERR_ABORT		(1 << 0)
#endif
#ifndef IRQ_LOCAL_LSERR_OVERFLOW
#define IRQ_LOCAL_LSERR_OVERFLOW	(1 << 1)
#endif
#ifndef IRQ_GENERATE_SERR
#define IRQ_GENERATE_SERR			(1 << 2)
#endif
#ifndef IRQ_MAILBOX_ENABLE
#define IRQ_MAILBOX_ENABLE			(1 << 3)
#endif
#ifndef IRQ_RESERVED1
#define IRQ_RESERVED1				(1 << 4)
#endif
#ifndef IRQ_RESERVED2
#define IRQ_RESERVED2				(1 << 5)
#endif
#ifndef IRQ_RESERVED3
#define IRQ_RESERVED3				(1 << 6)
#endif
#ifndef IRQ_RESERVED4
#define IRQ_RESERVED4				(1 << 7)
#endif
#ifndef IRQ_PCI_ENABLE
#define IRQ_PCI_ENABLE				(1 << 8)
#endif
#ifndef IRQ_PCI_DOORBELL_ENABLE
#define IRQ_PCI_DOORBELL_ENABLE		(1 << 9)
#endif
#ifndef IRQ_ABORT_ENABLE
#define IRQ_ABORT_ENABLE			(1 << 10)
#endif
#ifndef IRQ_LOCAL_PCI_ENABLE
#define IRQ_LOCAL_PCI_ENABLE		(1 << 11)
#endif
#ifndef IRQ_RETRY_ENABLE
#define IRQ_RETRY_ENABLE			(1 << 12)
#endif
#ifndef IRQ_PCI_DOORBELL_ACTIVE
#define IRQ_PCI_DOORBELL_ACTIVE		(1 << 13)
#endif
#ifndef IRQ_ABORT_ACTIVE
#define IRQ_ABORT_ACTIVE			(1 << 14)
#endif
#ifndef IRQ_LOCAL_ACTIVE
#define IRQ_LOCAL_ACTIVE			(1 << 15)
#endif
#ifndef IRQ_LOCAL_ENABLE
#define IRQ_LOCAL_ENABLE			(1 << 16)
#endif
#ifndef IRQ_LOCAL_DOORBELL_ENABLE
#define IRQ_LOCAL_DOORBELL_ENABLE	(1 << 17)
#endif
#ifndef IRQ_DMA_0_ENABLE
#define IRQ_DMA_0_ENABLE			(1 << 18)
#endif
#ifndef IRQ_DMA_1_ENABLE
#define IRQ_DMA_1_ENABLE			(1 << 19)
#endif
#ifndef IRQ_LOCAL_DOORBELL_ACTIVE
#define IRQ_LOCAL_DOORBELL_ACTIVE	(1 << 20)
#endif
#ifndef IRQ_DMA_0_ACTIVE
#define IRQ_DMA_0_ACTIVE			(1 << 21)
#endif
#ifndef IRQ_DMA_1_ACTIVE
#define IRQ_DMA_1_ACTIVE			(1 << 22)
#endif
#ifndef IRQ_BIST_ACTIVE
#define IRQ_BIST_ACTIVE				(1 << 23)
#endif
#ifndef IRQ_MASTER_ABORT
#define IRQ_MASTER_ABORT			(1 << 24)
#endif
#ifndef IRQ_DMA_0_ABORT
#define IRQ_DMA_0_ABORT				(1 << 25)
#endif
#ifndef IRQ_DMA_1_ABORT
#define IRQ_DMA_1_ABORT				(1 << 26)
#endif
#ifndef IRQ_TARGET_ABORT
#define IRQ_TARGET_ABORT			(1 << 27)
#endif
#ifndef IRQ_MAILBOX_0
#define IRQ_MAILBOX_0				(1 << 28)
#endif
#ifndef IRQ_MAILBOX_1
#define IRQ_MAILBOX_1				(1 << 29)
#endif
#ifndef IRQ_MAILBOX_2
#define IRQ_MAILBOX_2				(1 << 30)
#endif
#ifndef IRQ_MAILBOX_3
#define IRQ_MAILBOX_3				(1 << 31)
#endif


#ifndef ENABLE_CHAINING
#define ENABLE_CHAINING (1 << 9)
#endif
#ifndef DMA_PCI_IRQ_SELECT
#define DMA_PCI_IRQ_SELECT (1 << 17)
#endif


// DMA Command/Status Register Masks. Channel 0 and 1
// share the same register, so care must be taken not to
// overwrite one while writing the other.

#ifndef DMA_PRESERVE_CHANNEL_MASK_0
#define DMA_PRESERVE_CHANNEL_MASK_0	0xff
#endif
#ifndef CH0_DMA_ENABLE_MASK
#define CH0_DMA_ENABLE_MASK		(1 << 0)
#endif
#ifndef CH0_DMA_START_MASK
#define CH0_DMA_START_MASK		(1 << 1)
#endif
#ifndef CH0_DMA_ABORT_MASK
#define CH0_DMA_ABORT_MASK		(1 << 2)
#endif
#ifndef CH0_DMA_CLEAR_IRQ_MASK
#define CH0_DMA_CLEAR_IRQ_MASK	(1 << 3)
#endif
#ifndef CH0_DMA_DONE_MASK
#define CH0_DMA_DONE_MASK		(1 << 4)
#endif

#ifndef DMA_PRESERVE_CHANNEL_MASK_1
#define DMA_PRESERVE_CHANNEL_MASK_1	(0xff << 8)
#endif
#ifndef CH1_DMA_ENABLE_MASK
#define CH1_DMA_ENABLE_MASK		(1 << 8)
#endif
#ifndef CH1_DMA_START_MASK
#define CH1_DMA_START_MASK		(1 << 9)
#endif
#ifndef CH1_DMA_ABORT_MASK
#define CH1_DMA_ABORT_MASK		(1 << 10)
#endif
#ifndef CH1_DMA_CLEAR_IRQ_MASK
#define CH1_DMA_CLEAR_IRQ_MASK	(1 << 11)
#endif
#ifndef CH1_DMA_DONE_MASK
#define CH1_DMA_DONE_MASK		(1 << 12)
#endif


// DMA Mode Register Masks. 
#ifndef DMA_MODE_DEFAULT
#define DMA_MODE_DEFAULT       0x00000D40
#endif
#ifndef DMA_MODE_CHAINING
#define DMA_MODE_CHAINING      0x00000200
#endif
#ifndef DMA_MODE_DEMAND_ENABLE
#define DMA_MODE_DEMAND_ENABLE 0x00001000
#endif


// DMA Descriptor Pointer Register Masks. 
#ifndef DPR_RW_SHIFT
#define DPR_RW_SHIFT            3
#endif
#ifndef DPR_INT_ON_DMA_COMPLETE
#define DPR_INT_ON_DMA_COMPLETE 0x0004
#endif
#ifndef DPR_END_OF_CHAIN
#define DPR_END_OF_CHAIN        0x0002
#endif

// DMA Command/Status Register Masks. 
#ifndef START_DMA_CMD
#define START_DMA_CMD      0x000B
#endif
#ifndef STOP_DMA_CMD_MASK
#define STOP_DMA_CMD_MASK  0xFF01
#endif
#ifndef ABORT_DMA_CMD_MASK
#define ABORT_DMA_CMD_MASK 0x0004
#endif

// DMA Command/Status Register Masks. Channel 0 and 1
// share the same register, so care must be taken not to
// overwrite one while writing the other.
#ifndef DMA_PRESERVE_CHANNEL_MASK_0
#define DMA_PRESERVE_CHANNEL_MASK_0	0xff
#endif
#ifndef CH0_DMA_ENABLE_MASK
#define CH0_DMA_ENABLE_MASK		(1 << 0)
#endif
#ifndef CH0_DMA_START_MASK
#define CH0_DMA_START_MASK		(1 << 1)
#endif
#ifndef CH0_DMA_ABORT_MASK
#define CH0_DMA_ABORT_MASK		(1 << 2)
#endif
#ifndef CH0_DMA_CLEAR_IRQ_MASK
#define CH0_DMA_CLEAR_IRQ_MASK	(1 << 3)
#endif
#ifndef CH0_DMA_DONE_MASK
#define CH0_DMA_DONE_MASK		(1 << 4)
#endif

#ifndef DMA_PRESERVE_CHANNEL_MASK_1
#define DMA_PRESERVE_CHANNEL_MASK_1	(0xff << 8)
#endif
#ifndef CH1_DMA_ENABLE_MASK
#define CH1_DMA_ENABLE_MASK		(1 << 8)
#endif
#ifndef CH1_DMA_START_MASK
#define CH1_DMA_START_MASK		(1 << 9)
#endif
#ifndef CH1_DMA_ABORT_MASK
#define CH1_DMA_ABORT_MASK		(1 << 10)
#endif
#ifndef CH1_DMA_CLEAR_IRQ_MASK
#define CH1_DMA_CLEAR_IRQ_MASK	(1 << 11)
#endif
#ifndef CH1_DMA_DONE_MASK
#define CH1_DMA_DONE_MASK		(1 << 12)
#endif

#define DESCRIPTOR_IN_PCI_SPACE	(1 << 0)
#define END_OF_CHAIN_MARKER		(1 << 1) 
#define IRQ_AFTER_COUNT			(1 << 2)
#define TRANSFER_TO_PCI			(1 << 3)

// These are the masks for the PCI DMA Control Registers. 
#ifndef DMA_CMD_STAT_DONE
#define DMA_CMD_STAT_DONE      0x00000010
#endif
#ifndef DMA_CMD_STAT_CMD_MASK
#define DMA_CMD_STAT_CMD_MASK  0x00000007
#endif
#ifndef DMA_CMD_STAT_INT_CLEAR
#define DMA_CMD_STAT_INT_CLEAR 0x00000008
#endif
#ifndef DMA_CMD_STAT_START
#define DMA_CMD_STAT_START     0x00000003
#endif
#ifndef DMA_CMD_STAT_ABORT
#define DMA_CMD_STAT_ABORT     0x00000004
#endif
#ifndef DMA_MODE_DONE_ENABLE
#define DMA_MODE_DONE_ENABLE   0x00000400
#endif

// These are the masks for the PCI DMA Control Registers. 
#ifndef DMA_CMD_STAT_DONE_CH0
#define DMA_CMD_STAT_DONE_CH0      0x0010
#endif
#ifndef DMA_CMD_STAT_CMD_MASK_CH0
#define DMA_CMD_STAT_CMD_MASK_CH0  0x0007
#endif
#ifndef DMA_CMD_STAT_INT_CLEAR_CH0
#define DMA_CMD_STAT_INT_CLEAR_CH0 0x0008
#endif
#ifndef DMA_CMD_STAT_ABORT_CH0
#define DMA_CMD_STAT_ABORT_CH0     0x0004
#endif
#ifndef DMA_CMD_STAT_START_CH0
#define DMA_CMD_STAT_START_CH0     0x0003
#endif
#ifndef DMA_CMD_STAT_DONE_CH1
#define DMA_CMD_STAT_DONE_CH1      0x1000
#endif
#ifndef DMA_CMD_STAT_CMD_MASK_CH1
#define DMA_CMD_STAT_CMD_MASK_CH1  0x0700
#endif
#ifndef DMA_CMD_STAT_INT_CLEAR_CH1
#define DMA_CMD_STAT_INT_CLEAR_CH1 0x0800
#endif
#ifndef DMA_CMD_STAT_ABORT_CH1
#define DMA_CMD_STAT_ABORT_CH1     0x0400
#endif
#ifndef DMA_CMD_STAT_START_CH1
#define DMA_CMD_STAT_START_CH1     0x0300
#endif
#ifndef DMA_MODE_DONE_ENABLE
#define DMA_MODE_DONE_ENABLE       0x00000400
#endif

/* ---- DMA command/status register bits byte-level access ---- */
#define DMA_CHAN_ENABLE             0x0001
#define DMA_CHAN_START              0x0002
#define DMA_CHAN_ABORT              0x0004
#define DMA_CHAN_CLR_INT            0x0008
#define DMA_CHAN_DONE               0x0010

/* ---- DMA command/status register bits for channel 0 ---- */
#define DMA0_ENABLE             0x0001
#define DMA0_START              0x0002
#define DMA0_ABORT              0x0004
#define DMA0_CLR_INT            0x0008
#define DMA0_DONE               0x0010
#define DMA0_MASK               0x001F

/* ---- DMA command/status register bits for channel 0 ---- */
#define DMA1_ENABLE             0x0100
#define DMA1_START              0x0200
#define DMA1_ABORT              0x0400
#define DMA1_CLR_INT            0x0800
#define DMA1_DONE               0x1000
#define DMA1_MASK               0x1F00

#ifndef SETUP_DMA_CMD_0
#define SETUP_DMA_CMD_0         0x09
#endif

#ifndef STARTUP_DMA_CMD_0
#define STARTUP_DMA_CMD_0       0x03
#endif

#ifndef START_DMA_CMD_1
#define START_DMA_CMD_1         0xB00
#endif

#ifndef STOP_DMA_CMD_0_MASK
#define STOP_DMA_CMD_0_MASK     0x0C
#endif

#ifndef STOP_DMA_CMD_1_MASK
#define STOP_DMA_CMD_1_MASK     (0x0C<<16)
#endif

#ifndef DEMAND_DMA_MODE_READ
#define DEMAND_DMA_MODE_READ 	0x00001840
#endif


#ifndef NON_DEMAND_DMA_MODE
#define NON_DEMAND_DMA_MODE     0x00020D43
#endif

#ifndef DEMAND_DMA_MODE_WRITE
#define DEMAND_DMA_MODE_WRITE         (NON_DEMAND_DMA_MODE|1<<12)
#endif

#ifndef DMA_CMD_STAT_INT_CLEAR
#define DMA_CMD_STAT_INT_CLEAR  0x08
#endif

#ifndef PCI_INT_ENABLE
#define PCI_INT_ENABLE          0x00020D43
#endif

#endif // #ifndef PLX_REGS
