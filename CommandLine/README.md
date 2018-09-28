# CommandLine

A library for easily loading the command line into variables. Variables, either global or local in
scope context, are directly connected to command line options and automatically loaded.

## Glossary

An **option** is an optional item appearing on the command line, preceded by '-'. May require (a fixed number of) parameters.

A **parameter** is an item, perhaps one of several, connected to an option.

An **argument** is a non-option item on the command line. Can be fixed in number or the system can be configured to collect the "overflow" arguments.


