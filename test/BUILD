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

cc_test(
    name = "unique_padding",
    size = "small",
    srcs = [
        "unique_padding_test.cc",
        "std_array_comparison_operators.h",
    ] + glob([
        "test_data/*",
    ]),
    deps = [
        "//include:little_pp",
        "@googletest//:gtest_main",
    ],
)
