---
layout: default
title: "Prepare the HW and SW environment"
nav_order: 2
has_children: true
---

# Hardware Setup: i.MX RT1060 EVKC Rev C

The Revision C board is a crossover MCU evaluation kit featuring the **Cortex-M7** core running at **600MHz**.

### **Jumper Configuration (SW4)**
For internal boot from the onboard QSPI Flash, set the dip switches as follows:

| Switch | State | Purpose |
| :--- | :--- | :--- |
| SW4 [1-2] | OFF | Boot Mode: Internal |
| SW4 [3] | ON | Internal Boot |
| SW4 [4] | OFF | Standard Mode |

### **Power & Debugging**
* **Port J41:** Connect for both 5V power and CMSIS-DAP debugging.
* **UART:** The Virtual COM port operates at **115200 baud** by default.

# Software Environment

### **Toolchain**
* **IDE:** MCUXpresso IDE v25.06 ([Build 136] [2025-06-27]).
* **SDK:** Version 2.13+ (Targeting MIMXRT1062xxxxB) 
* follow on screen instructions from [MCUXpresso SDK Builder](https://mcuxpresso.nxp.com).
* I am using *SDK_2.x_MIMXRT1060-EVKC* - SDK version - *26.03.00*.

### **Troubleshooting the SDK Import**
When importing the SDK zip, a manifest error regarding the `RW612` middleware may appear.
* **Cause:** Incompatible metadata for secondary chips.
* **Fix:** Delete the error and proceed; the RT1060 drivers will still install correctly. Verify via the "Installed SDKs" tab.