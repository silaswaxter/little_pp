cc_library(
    name = "little_pp",
    srcs = glob(["impl/*.h", "*.cc"]),
    hdrs = [
        "data_model.h",
        "little_pp.h",
        "padding_reflection.h",
        "unique_padding_element.h",
    ],
    visibility = ["//visibility:public"],
    deps = [":boost_pfr"],
)

cc_library(
    name = "boost_pfr",
    srcs = [],
    hdrs = glob(["impl/3rd_party/pfr/include/boost/**/*.hpp"]),
    strip_include_prefix = "impl/3rd_party/pfr/include",
)
