# Kimoyo Oju Test & CI Plan

## 1. Overview

This document outlines the testing strategy for ensuring stability, memory safety, and crash-free operation of the Kimoyo Oju renderer.

## 2. Test Categories

### 2.1 Unit Tests (C++)

Location: `packages/kimoyooju-engine/cpp/tests/`

```cpp
// Example: HandleRegistry tests
TEST(HandleRegistry, AllocateAndRetrieve) {
    HandleRegistry<int> registry(100);
    
    Handle h = registry.allocate(42);
    EXPECT_TRUE(h.isValid());
    
    auto val = registry.get(h);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(**val, 42);
}

TEST(HandleRegistry, GenerationPreventsUseAfterFree) {
    HandleRegistry<int> registry(100);
    
    Handle h1 = registry.allocate(42);
    EXPECT_TRUE(registry.release(h1));
    
    // Reuse the slot
    Handle h2 = registry.allocate(99);
    
    // Old handle should fail (generation mismatch)
    auto val = registry.get(h1);
    EXPECT_FALSE(val.has_value());
    
    // New handle should work
    val = registry.get(h2);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(**val, 99);
}

TEST(CommandBufferValidator, RejectsNaN) {
    CommandBufferValidator validator;
    CommandBuffer buffer;
    buffer.version = 1;
    buffer.commands.push_back(SetTransformCmd{
        .handle = {1, 1},
        .position = {NAN, 0, 0},
        .rotation = {0, 0, 0, 1},
        .scale = {1, 1, 1}
    });
    
    auto result = validator.validate(buffer);
    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.errorCode, "INVALID_POSITION");
}
```

### 2.2 Integration Tests (TypeScript)

Location: `packages/react-native-kimoyo-oju/__tests__/`

```typescript
describe('CommandBufferBuilder', () => {
  it('builds valid command buffer', () => {
    const builder = new CommandBufferBuilder()
      .createNode({ id: 1, gen: 1 }, 'mesh', null)
      .setTransform({ id: 1, gen: 1 }, [0, 0, 0], [0, 0, 0, 1], [1, 1, 1]);
    
    const buffer = builder.build();
    
    expect(buffer.version).toBe(1);
    expect(buffer.commands.length).toBe(2);
    expect(buffer.commands[0].type).toBe('CREATE_NODE');
  });
});
```

### 2.3 Soak Tests (Native)

Location: `packages/kimoyooju-engine/cpp/tests/soak/`

These tests run hundreds of iterations to detect:
- Memory leaks
- Handle exhaustion
- Race conditions
- Resource cleanup issues

```cpp
// Soak test: enterXR/exitXR loops
TEST(SoakTest, EnterExitXRLoop) {
    auto engine = Engine::create({});
    auto initialCounters = engine->getMemoryCounters().snapshot();
    
    constexpr int ITERATIONS = 500;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        // Enter XR
        auto enterResult = engine->enterXR(XRMode::IMMERSIVE_VR);
        if (!enterResult.ok) {
            // XR may not be available in test enkimoyoojunment
            GTEST_SKIP() << "XR not available";
        }
        
        // Render a few frames
        for (int f = 0; f < 10; ++f) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        // Exit XR
        auto exitResult = engine->exitXR();
        ASSERT_TRUE(exitResult.ok) << "Failed at iteration " << i;
        
        // Check for leaks periodically
        if (i % 100 == 0) {
            auto counters = engine->getMemoryCounters().snapshot();
            EXPECT_LE(counters.liveSwapchains, initialCounters.liveSwapchains + 1);
        }
    }
    
    // Final leak check
    engine.reset();
    auto finalCounters = globalMemoryCounters().snapshot();
    EXPECT_EQ(finalCounters.liveNodes, 0);
    EXPECT_EQ(finalCounters.liveTextures, 0);
    EXPECT_EQ(finalCounters.liveSwapchains, 0);
}

// Soak test: background/foreground loops
TEST(SoakTest, PauseResumeLoop) {
    auto engine = Engine::create({});
    
    // Create some scene content
    for (int i = 0; i < 100; ++i) {
        auto result = engine->createNode(NodeType::MESH);
        ASSERT_TRUE(result.ok);
    }
    
    auto initialCounters = engine->getMemoryCounters().snapshot();
    
    constexpr int ITERATIONS = 200;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        auto pauseResult = engine->pause();
        ASSERT_TRUE(pauseResult.ok) << "Pause failed at iteration " << i;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        auto resumeResult = engine->resume();
        ASSERT_TRUE(resumeResult.ok) << "Resume failed at iteration " << i;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Verify no memory growth
    auto finalCounters = engine->getMemoryCounters().snapshot();
    EXPECT_EQ(finalCounters.liveNodes, initialCounters.liveNodes);
}

// Soak test: surface recreate loops
TEST(SoakTest, SurfaceRecreateLoop) {
    auto engine = Engine::create({});
    
    constexpr int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        // Simulate surface creation
        void* fakeSurface = reinterpret_cast<void*>(0x12345678 + i);
        auto attachResult = engine->attachSurface(fakeSurface, 1920, 1080);
        // May fail in test enkimoyoojunment without real surface
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Simulate surface destruction
        auto detachResult = engine->detachSurface();
        ASSERT_TRUE(detachResult.ok) << "Detach failed at iteration " << i;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Verify cleanup
    engine.reset();
    EXPECT_FALSE(globalMemoryCounters().hasLeaks());
}

// Soak test: rapid node create/destroy
TEST(SoakTest, NodeChurn) {
    auto engine = Engine::create({.maxNodes = 10000});
    
    std::vector<Handle> handles;
    
    constexpr int ITERATIONS = 1000;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        // Create batch
        for (int j = 0; j < 50; ++j) {
            auto result = engine->createNode(NodeType::MESH);
            if (result.ok) {
                handles.push_back(result.value);
            }
        }
        
        // Destroy some
        int toDestroy = std::min<int>(handles.size(), 30);
        for (int j = 0; j < toDestroy; ++j) {
            engine->destroyNode(handles.back());
            handles.pop_back();
        }
    }
    
    // Cleanup all remaining
    for (auto h : handles) {
        engine->destroyNode(h);
    }
    handles.clear();
    
    // Process cleanup queue
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto counters = engine->getMemoryCounters().snapshot();
    EXPECT_EQ(counters.liveNodes, 0);
}
```

## 3. Memory Tracking

### 3.1 Counter-Based Leak Detection

```cpp
class LeakDetector {
public:
    LeakDetector() : initial_(globalMemoryCounters().snapshot()) {}
    
    ~LeakDetector() {
        auto final_ = globalMemoryCounters().snapshot();
        
        if (final_.liveNodes > initial_.liveNodes) {
            ADD_FAILURE() << "Node leak: " 
                          << (final_.liveNodes - initial_.liveNodes) << " nodes";
        }
        if (final_.liveTextures > initial_.liveTextures) {
            ADD_FAILURE() << "Texture leak: "
                          << (final_.liveTextures - initial_.liveTextures) << " textures";
        }
        // ... etc
    }
    
private:
    MemoryCounters::Snapshot initial_;
};

// Usage in tests
TEST(SomeTest, DoesNotLeak) {
    LeakDetector detector;
    
    // ... test code ...
}
```

### 3.2 ASAN/UBSAN Builds

CMake configuration:

```cmake
option(KIMOYOOJU_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(KIMOYOOJU_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if(KIMOYOOJU_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(KIMOYOOJU_ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()
```

## 4. CI Pipeline

### 4.1 GitHub Actions Workflow

```yaml
name: CI

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: '20'
      - run: yarn install
      - run: yarn lint
      - run: yarn typescript

  test-cpp:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake ninja-build
      - name: Build
        run: |
          cmake -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=Debug \
            -DKIMOYOOJU_ENABLE_ASAN=ON \
            -DKIMOYOOJU_ENABLE_UBSAN=ON
          cmake --build build
      - name: Run unit tests
        run: ./build/tests/kimoyooju-engine-tests
      - name: Run soak tests
        run: ./build/tests/kimoyooju-engine-soak-tests
        timeout-minutes: 30

  test-cpp-release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake ninja-build
      - name: Build Release
        run: |
          cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
          cmake --build build
      - name: Run performance tests
        run: ./build/tests/kimoyooju-engine-perf-tests

  test-android:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-java@v4
        with:
          distribution: 'zulu'
          java-version: '17'
      - name: Build Android
        run: |
          cd apps/example/android
          ./gradlew assembleDebug

  test-ios:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: '20'
      - run: yarn install
      - name: Install CocoaPods
        run: |
          cd apps/example/ios
          pod install
      - name: Build iOS
        run: |
          cd apps/example/ios
          xcodebuild -workspace KimoyoOjuExample.xcworkspace \
            -scheme KimoyoOjuExample \
            -sdk iphonesimulator \
            -configuration Debug \
            build

  soak-test-device:
    runs-on: [self-hosted, quest]
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    steps:
      - uses: actions/checkout@v4
      - name: Build Quest APK
        run: |
          cd apps/example/android
          ./gradlew assembleQuest
      - name: Install and run soak tests
        run: |
          adb install -r app/build/outputs/apk/quest/debug/app-quest-debug.apk
          adb shell am instrument -w \
            -e iterations 500 \
            com.kimoyoojuexample.test/androidx.test.runner.AndroidJUnitRunner
      - name: Collect metrics
        run: |
          adb shell dumpsys meminfo com.kimoyoojuexample > meminfo.txt
          cat meminfo.txt
```

## 5. Test Metrics & Pass Criteria

### 5.1 Memory Growth Limits

| Metric | Threshold | Action if Exceeded |
|--------|-----------|-------------------|
| Node count growth | 0 after full cleanup | Fail test |
| Texture count growth | 0 after full cleanup | Fail test |
| Swapchain growth | 0 after XR exit | Fail test |
| GPU memory growth | < 1MB over 500 iterations | Warning |
| Native heap growth | < 5MB over 500 iterations | Warning |

### 5.2 Performance Baselines

| Operation | Target | Max |
|-----------|--------|-----|
| Command buffer submit (100 cmds) | < 1ms | 5ms |
| Node create | < 0.1ms | 1ms |
| Transform update | < 0.01ms | 0.1ms |
| XR frame (excluding render) | < 2ms | 5ms |

## 6. Regression Detection

### 6.1 Automated Bisect

When a soak test fails:

1. CI triggers automatic bisect to find culprit commit
2. Results posted to PR/issue
3. Culprit commit author notified

### 6.2 Memory Trend Tracking

```
Weekly memory tracking job:
1. Run full soak suite
2. Record peak memory usage
3. Compare to previous week
4. Alert if > 10% increase
```

## 7. Manual Test Checklist

Before each release:

- [ ] XR enter/exit 50 times on Quest 3
- [ ] Background/foreground 50 times on Quest 3
- [ ] Background/foreground 50 times on Android phone
- [ ] Background/foreground 50 times on iOS
- [ ] Guardian boundary trigger (Quest)
- [ ] Cable disconnect during XR (Quest)
- [ ] Low battery behavior
- [ ] Memory pressure (open many other apps)
