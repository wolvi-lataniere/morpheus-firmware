.. include :: ./doc/shared.rst

Morpheus Firmware project
#########################

*Morpheus* is a power management companion for your Single Board Computer projects. It is designed to be deployed alongside *Morpheus block* using `Balena Blocks <https://www.balena.io/docs/learn/more/examples/balenablocks/>`_.

Debugging
*********

To debug this project, set the OpenOCD server IP address to
`OPENOCD_SERVER_ADDRESS` environment variable before running VSCode:

.. code:: bash

  export OPENOCD_SERVER_ADDRESS=192.168.9.101

Project status
**************

* |check| Blink a LED
* |uncheck| Communicate with UART
* |uncheck| Integrate debugging with VSCode
* |uncheck| Build the firmware structure
* |uncheck| Define protocol
* **Implementation**
  
 * |uncheck| Interrupt based UART
 * |uncheck| Protocol handling class
 * |uncheck| Power management class
 * |uncheck| *Morpheus* service
 
* **Documentation**
  
 * |uncheck| Document structure
 * |uncheck| Document installation
 
