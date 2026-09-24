# Raptor Expiremental Device
*Clever Girl*

An experimental zigbee firmware, with the aim of allowing devices to perform complex behivour without the need for a cordinator. Raptor should allow users to configure complex behivour without having to write there own firmware.

## The Problem

Currently complex behaviour requires the central cordinator. I see this as a weak point: if the cordinator is down you can't turn your lights on.

For example, a standard Zigbee2mqtt & Home Assistant setup for making a light switch turn on different light bulbs during the day and night is:
1. Switch is pressed and message is sent to cordinator (z2mqtt)
2. z2mqtt publishes a message on a mqtt topic
3. HA picks up message and runs automation, automation decides which bulbs should be turned on and publishes messages to correct mqtt queues
4. z2mqtt picks up messages and sends zigbee commands to bulbs
5. Bulbs turn on

What if the cordinator is down? What if HA is down? What if the mqtt server is down? You can't turn your lights on. Worse, if lots of your smart home works like this then many things won't work.

## Zigbee Binding

Zigbee binding helps to improve this. When the switch is direct binded to a bulb it sends commands directly to the bulb. The only devices needed are the switch and the bulb (plus any routers in between if the devices are not neighbours).

However complex behaviour, such as different lights in the day vs the night, would require the user to write their own firmware. How would that custom firmware work?

The device could provide 2 endpoints with the output/client onOff cluster, 1 for day lights and 1 for night lights. You bind the bulbs you want on during the day to endpoint 1 and the night bulbs to endpoint 2. The device would also need to know the current time, this could be set by a custom config attribute (? might need more thought). Then when the switch is pressed the device could decide which set of binded devices to send the command to depending on the current time.

Raptor aims to make this achievable without having to write custom firmware.

## How?

The firmware is provided a config which defines **blocks** and **links**.

Blocks are:
1. **Peripherals** (Input devices such as Switch and Rotary Encoder) which expose callbacks (outputs) when events happen, such as `button_pressed`
2. **Clusters** (Zigbee Clusters such as genOnOff) which expose actions (inputs) for client clusters and callbacks (outputs) for server clusters
3. **Logic Blocks** (such as If statements & transformers) which expose actions (inputs) and callbacks (outputs)

Links define how these blocks are connected: `callback(output) -> action(input)`

Day and Night Bulb Example:
```
 +-----------------------+
 |  Switch Peripheral    |
 +-----------------------+        +--------------------------+
 | on_toggle >--------------------> trigger                  |
 +-----------------------+        |                          |
                                  |  Transformer             |
 +-----------------------+        |  (add second param)      |
 |  Time                 |        |                          |
 +-----------------------+        |                          |
 | get_current_time <-------------< fetch_value              |
 +-----------------------+        |                          |
                                  | output >---------+       |
                                  +--------------------------+
                                                     |
                                                     v
                                  +--------------------------+
                                  |  If Statement            |
                                  |  (is daytime?)           |
                                  +--------------------------+
                                  | on_true >---------+      |
                                  | on_false >-----+  |      |
                                  +----------------|---------+
                                                   |  |
                            +----------------------+  |
                            |                         |
                            v                         v
 +---------------------------+    +---------------------------+
 |  genOnOff Cluster (ep 2)  |    |  genOnOff Cluster (ep 1)  |
 +---------------------------+    +---------------------------+
 | sendToggleCmd             |    | sendToggleCmd             |
 +---------------------------+    +---------------------------+
   (night bulbs)                    (day bulbs)
```