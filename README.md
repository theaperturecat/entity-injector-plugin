# Source Engine Entity Adder Plugin
It seems no one has ever released a plugin that can do this (at least not the full source code) so I decided to make one.
It's not perfect, but at least it is good enough to add a simple entity to portal 2.

As well as adding entities, this plugin also adds 2 console commands:

* **dump_full_datadesc** {entity name} - dumps all save restore variables and keyvalues

* **read_datadesc_val** {entity name} {value} - reads a single datadesc value from an entity

This only works with the 2014 build of portal 2 with PDB debug symbols.
