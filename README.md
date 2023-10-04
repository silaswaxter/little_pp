# About

LittlePP is a serialization/deserialization header-only library for C++ which is
**little** in size, performance cost, and API complexity. LittlePP generates
(through templates) the serialization/deserialization methods for class types
(ie `class` and `struct`) with optional, built-in padding and endianess
conversion. LittlePP can handle nested class types and works on plain-old-data
(POD) except pointers and references types (more specifics below).

LittlePP was written because I was writing an embedded C++ driver for a
coprocessor for which I had no control of its software interface; all of the
software-defined "registers" expected its architecture's data model (ie its
padding schema and little-endian data). I wanted to write a generic driver which
does the "right thing" regardless of whether I'm running on a 16-bit, big-endian
processor or a 32-bit, little-endian processor. LittlePP allows the driver to
define the coprocessor's expected data model, inspect (at compile time) the host
proccessor's data model, and depend on LittlePP to do the "right thing" during
serialization/deserialization.

## Requirements

## Examples

## How to get setup

Since Boost::pfr is a direct dependency of this library, and its tracked as a
git submodule, your build system must be instructed where to search for the pfr
headers (ie `-I<location-of-this-library>/3rd_party/pfr/include`).

## Contributing

### Goals and Design Decisions
