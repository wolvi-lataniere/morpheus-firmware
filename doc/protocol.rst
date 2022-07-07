Morpheus protocol description
#############################

This document describes the UART protocol used in the Morpheus project.

UART Configuration
******************

The communication between the micro-controller unit (MCU) and the single
board computer (SBC) is based on UART communication with following
specification:

- **Speed:** 115 200 bps
- **Word size:** 8 bits
- **Parity:** None
- **Stop bits:** 1 bit
- **Flow control:** None

Frame format
************

The communication is based on a frame exchange format.
A single frame is composed as follows:

========== =================================== ================ =========================
   Name     Description                         Size(in bytes)    Value (if applicable)
---------- ----------------------------------- ---------------- -------------------------
**Header**  Start of frame delimiter           2                0x55 0xAA
**Size**    Size of frame (incl. header)       1                0 - 255
**Code**    Inscruction code (see Code_)       1                see Code_
**Params**  Instruction parameters (see Code_) 1-250            see Code_  
**Chksum**  Frame checksum (see Checksum_)     0-255            see Checksum_
========== =================================== ================ =========================

Code
----

*Codes* have two kinds of values:

- an insctruction descriptor when sent **TO the MCU FROM the SBC**,
- a feedback descriptow when sent **FROM the MCU TO the SBC**.

The meaning is only defined by the direction in this case (a single code
can be used for the instruction and its feedback).

**NOTE:** Instruction value `0xFF` is reserved for `Extended codes`_.

Currently registered codes are available `here <../primer/codes.yml>`_.

Extended codes
++++++++++++++

Extender codes can be used if the available codes range is saturated.
An extended code behavies like normal codes, except they are 2 bytes long,
with first byte equal to `0xFF`.

Checksum
--------

The *checksum* is a 8 bits troncated, zero-seeded sum of all
the **Size**, **Code** and **Params** data.

In pseudo code:
::

  checksum = size + code
  foreach d in param:
    checksum = checksum + d
  checksum = checksum & 0xFF


