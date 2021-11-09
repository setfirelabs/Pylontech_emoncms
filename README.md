## NodeMCU / MCP2515_CAN Sketch for a system of Pylontech US2000 / US3000 batteries connected to Sofar Solar ME3000SP inverter/charger with data output to emoncms.
## Warning first
Messing around with your battery system is not a good idea unless you really, really, know what you are doing. You will also most likely INVALIDATE ANY WARRANTIES.

While part of this sketch is designed to replicate CAN data to additional inverter/chargers, IT IS YOUR RESPONSIBILITY to ensure the system behaves correctly and the batteries are properly managed within their operational parameters.
Don't come bleating if you:

* destroy your battery system.
* destroy your inverter/chargers.
* burn your house down.
* do anything worse.

**YOU HAVE BEEN WARNED!**

Also bear in mind that the sketch as it stands does not modify *any* data from the batteries sent to the inverter/chargers. This means that the current limits for charging and discharging will be sent to the inverter/charger as if it is the only one. If you have 3x inverter/chargers on 3x CAN buses, you therefore have the potential to send 3x the maximum current to the batteries, and you should make sure your system and especially cables are suitable for this, or adjusted accordingly. **IF YOU DON'T UNDERSTAND THIS POINT FULLY, YOU SHOULD NOT BE USING THIS SKETCH.**

More info about this sketch can be found here: https://setfirelabs.com/
