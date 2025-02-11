.. SPDX-License-Identifier: GPL-2.0+
.. Copyright (C) 2025 Texas Instruments Incorporated, <www.ti.com>

.. index::
   single: fuse writebuff (command)

fuse writebuff command
======================

Synopsis
--------

::

    fuse writebuff [-y] <addr>

Description
-----------

Program fuse data using a structured buffer in memory starting at 'addr'.
The option -y confirms the operation without prompting for confirmation.
The parameter <addr> is the memory address of the structured buffer containing the fuse data.

The structured buffer should contain all necessary details for programming 
fuses, such as the values to be written to the fuse, optional metadata for 
validation or programming constraints and any configuration data required for 
the operation.

Examples
--------

fuse writebuff 0x84000000
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example programs fuses using a structured buffer located at address 0x84000000,
on AM62L.
::

     u-boot=> fatload mmc 1:1 0x84000000 fuse_buff.bin
     544 bytes read in 3 ms (176.8 KiB/s)
     u-boot=> fuse writebuff 0x84000000
     Programming fuses with buffer at addr 0x84000000
     Warning: Programming fuses is an irreversible operation!
              This may brick your system.
              Use this command only if you are sure of what you are doing!

     Really perform this fuse programming? <y/N>
     y

fuse writebuff -y 0x84000000
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example confirms the fuse programming operation without prompting, on AM62L.
::

     u-boot=> fatload mmc 1:1 0x84000000 fuse_buff.bin
     544 bytes read in 3 ms (176.8 KiB/s)
     u-boot=> fuse writebuff -y 0x84000000
     Programming fuses with buffer at addr 0x84000000

Configuration
-------------

The fuse writebuff command is available if CONFIG_K3_FUSE=y.

Return code
-----------

The return value $? is set to 0 (true) if the command is successful,
1 (false) otherwise.
