########
CIFAR-10
########

This example application implements a CNN architecture trained on the `CIFAR-10 <https://www.cs.toronto.edu/~kriz/cifar.html>`__ dataset.  This example demonstrates how to place models in SRAM, Flash or LPDDR.

The example reads a set of test images from a filesystem in flash.  The FreeRTOS kernel manages filesystem IO and sends example images to the AI device that implements the CIFAR-10 model.  The application will attempt to classify an entity in the image and assign it to one of the following classes:

- airplane
- automobile
- bird
- cat
- deer
- dog
- frog
- horse
- ship
- truck

The resulting output tensor is returned to FreeRTOS for the application to handle.

A Python script is provided that will generate one example image from each of the classes above.  Ensure you have installed Python 3 and the XCore SDK Python requirements.

To generate the images run:

.. tab:: Linux and Mac

    .. code-block:: console

        $ cd filesystem_support/test_inputs
        $ ./make_test_tensors.py
        $ cd ../..
        
.. tab:: Windows XTC Tools CMD prompt

    .. code-block:: console
    
        $ cd filesystem_support\test_inputs
        $ python3 make_test_tensors.py
        $ cd..\..

For background information on the CIFAR-10 dataset, please read `Learning Multiple Layers of Features from Tiny Images <https://www.cs.toronto.edu/~kriz/learning-features-2009-TR.pdf>`__, Alex Krizhevsky, 2009.

****************
Supported Boards
****************

This example is supported on the XCORE-AI-EXPLORER board.

.. note::

    The external DDR memory options are only available on the XCORE-AI-EXPLORER board.

*********************
Building the firmware
*********************

Using SRAM memory
=================

.. tab:: Linux and Mac

    Run make:

    .. code-block:: console

        $ cmake -B build -DBOARD=XCORE-AI-EXPLORER
        $ cd build
        $ make

    To flash the example images, run the following commands:

    .. code-block:: console

        $ make fsonly BOARD=XCORE-AI-EXPLORER
        
.. tab:: Windows XTC Tools CMD prompt

    .. code-block:: console
    
        $ cmake -G "NMake Makefiles" -B build -DBOARD=XCORE-AI-EXPLORER
        $ cd build
        $ nmake

Using external flash memory
===========================

To build with the model stored in flash, replace the call to make above with the following:

.. tab:: Linux and Mac

    .. code-block:: console

        $ make BOARD=$TARGET USE_SWMEM=1

To flash the model and example images, run the following commands:

.. tab:: Linux and Mac

    .. code-block:: console

        $ make swmem BOARD=XCORE-AI-EXPLORER

Using external DDR memory
=========================

If your board supports LPDDR, you may also place your neural network in the external DDR memory.  Currently, only the Explorer Board supports LPDDR.

To build with the model stored in LPDDR, replace the call to make above with the following:

.. tab:: Linux and Mac

    .. code-block:: console

        $ make BOARD=$TARGET USE_EXTMEM=1
        
To flash the example images, run the following commands:

.. tab:: Linux and Mac

    .. code-block:: console

        $ make fsonly BOARD=XCORE-AI-EXPLORER

Running the firmware
====================

Running with hardware.

.. code-block:: console

    $ xrun --xscope bin/cifar10.xe
        