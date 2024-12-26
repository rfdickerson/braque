//
// Created by rfdic on 12/18/2024.
//

// tests/test_renderer.cpp
#include "gtest/gtest.h"
#include "braque/braque.h"

TEST(RendererTest, Initialization) {
    braque::Renderer renderer;
    // ... your assertions to test renderer initialization ...
    auto device = renderer.getDevice();
    EXPECT_TRUE(device);
}

TEST(WindowTest, Initialization) {
    braque::Window window;
    // ... your assertions to test window initialization ...
    auto nativeWindow = window.GetNativeWindow();
    EXPECT_TRUE(nativeWindow);
}