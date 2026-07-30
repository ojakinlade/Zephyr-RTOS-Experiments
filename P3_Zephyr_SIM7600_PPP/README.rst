SIM7600 PPP MQTT Bring-up
#########################

Overview
********

This application targets a Raspberry Pi Pico running Zephyr with a SIM7600
connected on ``uart0``. It uses Zephyr's cellular modem PPP path to:

1. power and start the modem,
2. bring up the PPP network interface,
3. wait for IPv4 and DNS availability,
4. resolve ``broker.hivemq.com``,
5. connect over plain MQTT on port ``1883``,
6. publish a test payload.

This is intended as the first milestone before moving to AWS IoT over TLS.

Hardware Assumptions
********************

- USB CDC ACM remains the Zephyr console.
- SIM7600 UART is attached to Pico ``GPIO16`` (TX) and ``GPIO17`` (RX).
- ``mdm-power-gpios`` is currently mapped to Pico ``GPIO0`` in
  ``boards/rpi_pico_rp2040.overlay``.

If your SIM7600 board uses a different pin for PWRKEY or enable, update the
overlay before flashing.

Building and Running
********************

Set your APN in ``prj.conf``:

.. code-block:: cfg

   CONFIG_MODEM_CELLULAR_APN="your-apn"

Then build:

.. code-block:: shell

   make pristine
   make build

Sample Output
=============

.. code-block:: console

   <inf> sim7600_ppp_demo: Powering SIM7600 modem
   <inf> sim7600_ppp_demo: Bringing up PPP interface
   <inf> sim7600_ppp_demo: Waiting for PPP L4 connection
   <inf> sim7600_ppp_demo: Waiting for DNS server from modem
   <inf> sim7600_ppp_demo: Resolved broker.hivemq.com to ...
   <inf> sim7600_ppp_demo: MQTT connected to HiveMQ
   <inf> sim7600_ppp_demo: Publishing to topic: zephyr/pico/sim7600/test
   <inf> sim7600_ppp_demo: HiveMQ publish complete. PPP path is ready for AWS TLS next.
