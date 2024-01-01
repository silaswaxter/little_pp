cc_test(
    name = "padding_reflection",
    size = "small",
    srcs = [
        "padding_reflection_test.cc",
        "std_array_comparison_operators.h",
    ] + glob([
        "test_data/*",
    ]),
    deps = [
        "//include:little_pp",
        "@googletest//:gtest_main",
    ],
)
