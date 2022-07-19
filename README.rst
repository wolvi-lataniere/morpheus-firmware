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

* [X] Blink a LED
* [X] Communicate with UART
* [X] Integrate debugging with VSCode
* [X] Build the firmware structure
* [X] Define protocol
* **Implementation**
  
 * [X] Interrupt based UART
 * [X] Protocol handling class
 * [X] Power management class
 * [X] *Morpheus* service
 
* **Documentation**
  
 * [ ] Document structure
 * [ ] Document installation
 
