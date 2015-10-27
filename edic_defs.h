/**
 * @file edic_defs.h
 * @brief Definitions of EDIC hardware
 */
#ifndef EDIC_DEFS_H
#define EDIC_DEFS_H

#define _b(shift) ((uint32_t)1 << shift)
#define MREG(addr) (*(volatile uint32_t*)(addr))
#define FLAG(sreg, mask) (((sreg) & (mask)) == (mask))

#define TIMER_FREQ 32768
#define TIMER_PWR_THRESHOLD 5 //sec

/** @name RAM definition
 * @{ */
#define REG_BASE 0x40000000
#define VMEM_BASE 0x30000000
#define SYSMEM_BASE 0x10000000
#define SYSMEM_SIZE 0x8000
/* @} */

/** @name CTRL register, R/W
 * @{ */
#define REG_CTRL_OFFSET 0x00
#define CTRL MREG(REG_BASE + REG_CTRL_OFFSET)

#define UART_BAUD_MASK 0x3FC0000
#define PSHCLK_MODE_MASK 0x30000
#define P0_MODE_MASK 0x8000
#define LEDFLASH_MODE_MASK 0x7000
#define LIN_ADDR_MASK 0x800
#define CAMCLK_INV_MASK 0x400
#define REFCLK_MODE_MASK 0x300
#define ITIMER_MODE_MASK 0x80
#define PI_MASK_MASK 0x70
#define HCLK_MODE_MASK 0x0C
#define VMEM_EN_MASK 0x02
#define GRAB_EN_MASK 0x01

#define UART_BAUD 18
#define PSHCLK_MODE 16
#define P0_MODE 15
#define LEDFLASH_MODE 12
#define LIN_ADDR 11
#define CAMCLK_INV 10
#define REFCLK_MODE 8
#define ITIMER_MODE 7
#define PI_MASK 4
#define HCLK_MODE 2
#define VMEM_EN 1
#define GRAB_EN 0

#define HCLK_MODE_DIV64 0x00
#define HCLK_MODE_DIV4 0x04
#define HCLK_MODE_DIV2 0x08
#define HCLK_MODE_NODIV 0x0c

#define LEDFLASH_MODE_ALWAYS_ON 0x3000
#define LEDFLASH_MODE_FRAME_ON 0x2000
#define LEDFLASH_MODE_LINE_ON 0x1000
/** @} */

/** @name WDT register
 * @{ */
#define REG_WDOG_OFFSET 0x04
#define WDOG MREG(REG_BASE + REG_WDOG_OFFSET)
/** @} */

/** @name PSH_DOUT register, W
 * @{ */
#define REG_PSH_DOUT_OFFSET 0x08
#define PSH_DOUT MREG(REG_BASE + REG_PSH_DOUT_OFFSET)
/** @} */

/** @name P1_DOUT register, W
 * @{ */
#define REG_P1_DOUT_OFFSET 0x0c
#define P1_DOUT MREG(REG_BASE + REG_P1_DOUT_OFFSET)
#define P1_DOUT_SCL 0x01
#define P1_DOUT_SDA 0x02
#define P1_DOUT_MOSI 0x04
#define P1_DOUT_SCK 0x08
#define P1_DOUT_CS_E 0x10
#define P1_DOUT_CS_F1 0x20
#define P1_DOUT_CS_R 0x40
#define P1_DOUT_ALE 0x80
#define P1_DOUT_CLE 0x100
#define P1_DOUT_PRF	0x200
#define P1_DOUT_CS_F2 0x400
/** @} */

/** @name VADDR_START register, W
 * @{ */
#define REG_VADDR_OFFSET 0x10
#define VADDR MREG(REG_BASE + REG_VADDR_OFFSET)

#define VADDR_START_V_MASK 0x00ff0000
#define VADDR_START_U_MASK 0x0000ff00
#define VADDR_START_Y_MASK 0x000000ff

#define VADDR_START_V 23
#define VADDR_START_U 15
#define VADDR_START_Y 7
/** @} */

/** ITIMER register address, W */
#define REG_ITIMER_OFFSET 0x14
#define ITIMER MREG(REG_BASE + REG_ITIMER_OFFSET)
/** @} */

/** @name TIMER register, R
 * @{ */
#define REG_TIMER_OFFSET 0x10
#define TIMER MREG(REG_BASE + REG_TIMER_OFFSET)
/** @} */

/** @name UART register, R/W
 * @{ */
#define REG_UART_OFFSET 0x18
#define UART MREG(REG_BASE + REG_UART_OFFSET)

#define UART_DEFAULT_SPD 1 //UART_SPD_115200
/** @} */

/** @nam
e STATUS register, R
 * @{ */
#define REG_STATUS_OFFSET 0x04
#define STATUS MREG(REG_BASE + REG_STATUS_OFFSET)

#define LUP_FLAG_MASK 0x200
#define WD_FLAG_MASK 0x100
#define BRK_FLAG_MASK 0x80
#define RX_DRDY_MASK 0x40
#define TX_BUSY_MASK 0x20
#define SHIFT_BUSY_MASK 0x10
#define LOW_BATT_MASK 0x0C
#define CAM_FRAME_MASK 0x02
#define FGRAB_MASK 0x01

#define LUP_FLAG 9
#define WD_FLAG 8
#define BRK_FLAG 7
#define RX_DRDY 6
#define TX_BUSY 5
#define SHIFT_BUSY 4
#define LOW_BATT0 2
#define CAM_FRAME 1
#define FGRAB 0
/** @} */

/** @name PSH_DIN register, R
 * @{ */
#define REG_PSH_DIN_OFFSET 0x08
#define PSH_DIN MREG(REG_BASE + REG_PSH_DIN_OFFSET)
/** @} */

/** @name P2_DIN register, W
 * @{ */
#define REG_P2DIN_OFFSET 0x0C
#define P2_DIN MREG(REG_BASE + REG_P2DIN_OFFSET)
/** @} */

/** @name Debug LEDs register, W
 * @{ */
#define REG_PLDEBG_DOUT_OFFSET 0x1c
#define PLDEBG_DOUT MREG(REG_BASE + REG_PLDEBG_DOUT_OFFSET)
/** @} */

/** @name Iterrupt ctrl registers
 * @{ */
#define ISER MREG(0xE000E100)
#define ICER MREG(0xE000E180)

/** @name P0 port, W/R
 * @{ */
#define PORT_P0_OFFSET 0x20000000
#define P0 MREG(PORT_P0_OFFSET)
/** @} */

#define IE_LOW_BATT 0
#define IE_PSH_DOUT 1
#define IE_P2_DIN 2
#define IE_ITIMER 3

#define IE_LOW_BATT_MASK 0x01
#define IE_PSH_DOUT_MASK 0x02
#define IE_P2_DIN_MASK 0x04
#define IE_ITIMER_MASK 0x08
#define IE_MASK (IE_LOW_BATT_MASK | IE_PSH_DOUT_MASK | IE_P2_DIN_MASK | IE_LOW_BATT_MASK)
/** @} */

#endif /* MC_DEFS_H */
