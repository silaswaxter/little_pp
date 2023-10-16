# LittlePP

LittlePP is a serialization/deserialization, header-only library for C++ that is
**_little_** in size, performance cost, and API complexity. LittlePP generates
(through templates) serialization/deserialization methods for class types (ie
`class` and `struct`). The API explicitly declares the data model (ie padding
schema and endianess) for both the input and output of the
serialization/deserialization methods; conversion between differing data models
is applied within the method automatically. LittlePP can handle nested class
types and works on plain-old-data (POD) except pointers and references types
(more specifics below).

## Motivation

LittlePP was written because I was writing an embedded C++ driver for a
coprocessor for which I had no control of the interface; all of the
software-defined "registers" expected its architecture's data model (ie the
ARM-Cortex-M4 padding schema and little-endian data). I wanted to write a
generic driver which does the "right thing" regardless of whether I'm running on
a 16-bit, big-endian processor or a 32-bit, little-endian processor. With
LittlePP, my device driver can define the coprocessor's expected data model and
depend on LittlePP to create efficient serialization/deserialization methods;
the driver is abstracted from the platform it runs on.

### Effects

- Make performance design decisions regarding serialization more easy to
  implement and change.

  - Consider an example in which a real-time system has two devices that are
    communicating with each other, and **you have control over both devices**.
    Endpoint A is heavily-burdened and its processing power is becoming a
    concern. Endpoint B has spare processing power. Lets explore how the system
    might be optimized within in terms of the serialization sub-system to reduce
    the burden on Endpoint A:
    - If there is **little margin** between real application data and the
      underlying serial stream throughput, the system is forced to communicate
      with zero-padding; the endpoints can decide whether converting to the
      native data model is necessary/good.
    - If there is **big margin** between real application data and the
      underlying serial stream throughput, the system can specify communication
      as one of its endpoint's data models.
      - In this case, since Endpoint A is more constrained, the stream can
        specify Endpoint A's native data model. This allows Endpoint A to have
        memory-aligned variables with no conversion runtime cost. Endpoint B may
        or may not have to convert to Endpoint A's data model.

- portability
  - C++ code can be written so that it doesn't implicitly depend on execution
    platform's data model.

### Illustrative Examples

**TODO:** create some examples on code explorer that illustrate:

- serialization
- deserialization
- endianess conversion
- padding conversion
- how it might be used in a device driver

## Requirements

- C++ Version >= C++14

## Installing

Boost::pfr (**not dependent on Boost, just released together**) is a direct
dependency of this library.

1. LittlePP tracks Boost::pfr as a git submodule. Therefore, you must populate
   it (i.e. `git submodule update --init --recursive`)
1. Your compiler must be pointed at Boost::pfr headers (e.g.
   `-I<location-of-this-library>/include/detail/3rd_party/pfr/include`).
   - One of the side effects of pfr releasing with boost is that it expects it
     to be located at `<boost/...>`, meaning LittlePP cannot include pfr for you
     using the relative path.
1. Your compiler must be pointed at LittlePP (e.g.
   `-I<location-of-this-library>/include`)

### About "Boost" in Boost::pfr

I'm weary of depending on a library with Boost in the name since its usage is
controversial. For example, the google style guide forbids all boost libraries
except a few. However, Boost::pfr is not apart of boost, its just released with
it. The project's README describes another repo called plain `pfr`, but its not
kept up-to-date so its necessary.

## Contributing

Use this GitHub repo for issues, PRs, etc. The following subsections describe
collaboration processes in terms of writing code and project direction.

### Standards

Some standards/best practices that are adhered to on a best-effort basis.

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
  - `clang-format` formatting (config file tracked)
- [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/)
  - `clang-tidy` linting (config file tracked)

### Setup the Test Environment

The build system used for testing the library is Bazel. Since, at the time of
writing, Bazel's default C++ toolchain is for c++2011, and AFAIK there isn't a
way to override this without resorting to the `copts` in BUILD targets, a basic
c++ toolchain is constructed; currently, the hermeticity is violates since this
toolchain depends on clang 16 being installed on a Unix machine--my setup. This
should be fixed, but I didn't want to waste time making the build system generic
if I'm the only one using it.

### Goals and Design Decisions

## License

Distributed under the [The MIT License](https://opensource.org/license/mit/).
