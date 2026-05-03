# Raptor Expiremental Device
Clever Girl 
An experimental zigbee firmware, with the aim of allowing devices to perform complex behivour without the need for a cordinator.  
Raptor should allow users to configure complex behivour without having to write there own firmware. 

Currently complex behaviour, requires the central cordinator. I see this as weak point: If the cordinator is down your can't turn your lights on. 

For Example: a standard Zigbee2mqtt & Home Assistant setup for making a light switch turn on different light bulbs during the day and night is:
1. Switch is pressed and message is sent to cordinator (z2mqtt)
2. z2mqtt publiish a message on a mqtt topic
3. HA picks up message and runs automation, automation decides which bulbs should be turned on and publishes messages to correct mqtt queues
4. z2mqtt picks up messages and sends zigbee commands to bulbs
5. Bulbs turn on
What if the cordinator is down? What if HA is down? What if the mqtt server is down? - You can't turn your lights on.
Worse, if lots of your smart home works like this then many things won't work. 

Zigbee binding help to improve this. When the switch is direct binded to a bulb is send commands directly to the bulb. The only devices needed are the switch and the bulb (plus any routers in between if the devices are not neigbours).
However complex behaviour, such as the different lights in the day vs the night would require the user to write there own firmware. 
How would that work custom firmware work? 
The device could provided 2 endpoints with the output/client onOff cluster, 1 for day lights and 1 for night lights. You bind the bulbs you want on during the day to endpoint 1 and the night bulbs to endpoint 2. The device would also need to know the current time, this could be set by a custom config attribute(? might need more thought). Then when the switch is pressed the device could decide which set of binded devices to send the command to depending on the current time. 
Raptor aims to make this achievable without having to write custom firmware.

How?
The firmware is provided a config which defines blocks and links.
Blocks are: 
1. Peripherals (Input devices such as Switch and Rotary Encoder) which expose callbacks (outputs) when events happen, such as button_pressed 
2. Clusters (Zigbe Clusters such as genOnOff) which expose actions (input) for client clusters and callbacks (output) for server clusters
3. Logic Blocks (such as If statements & transformers) which expose actions (input) and callbacks (outputs)
Links then define how these blocks are connected, callback(output)->action(input)

Day and Night Bulb Example:
Block                       Block                               
Switch Peripheral           Transformer, add second param       
on_toggle------------------>trigger
                            |
Block                       |
Time                        v
get_current_time<-----------fetch_value_for_second_param        Block
                            |                                   If Statement
                            v                                   Is current time daytime?        Block
                            output----------------------------->trigger                         genOnOffCluser endpoint 1
                                                                on_true------------------------>sendToggleCmd
                                                                |                        
                                                                |                               genOnOffCluser endpoint 2
                                                                on_false----------------------->sendToggleCmd