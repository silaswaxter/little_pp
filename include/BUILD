cc_library(
    name = "little_pp",
    srcs = [],
    hdrs = [
        "data_model.h",
        "little_pp.h",
    ],
    visibility = ["//visibility:public"],
    deps = [":boost_pfr"],
)

cc_library(
    name = "boost_pfr",
    srcs = [],
    hdrs = glob(["detail/3rd_party/pfr/include/boost/**/*.hpp"]),
    strip_include_prefix = "detail/3rd_party/pfr/include",
)
