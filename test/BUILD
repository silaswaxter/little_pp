cc_test(
    name = "little_pp",
    size = "small",
    srcs = [
        "little_pp_test.cc",
        "std_array_comparison_operators.h",
    ] + glob([
        "test_data/*",
    ]),
    deps = [
        "//include:little_pp",
        "@googletest//:gtest_main",
    ],
)
