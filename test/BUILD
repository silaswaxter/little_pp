cc_test(
    name = "little_pp",
    size = "small",
    srcs = [
        "little_pp_test.cc",
    ] + glob([
        "test_data/*",
    ]),
    deps = [
        "//include:little_pp",
        "@gtest//:gtest_main",
    ],
)
