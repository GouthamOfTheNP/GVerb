# GUIDELINES.md

This was somewhat written by GitHub Copilot (but it does express my ideas pretty well).

## Project Overview

`GVerb` is a JUCE-based audio plugin project that builds as VST3, AU, CLAP, and Standalone on macOS.  
The codebase is centered around:

- `src/GVerbProcessor.hpp/.cpp` for audio processing, parameter handling, and state management
- `src/GVerbEditor.hpp/.cpp` for the plugin UI
- `main.cpp` for plugin entry point registration
- `CMakeLists.txt` for build configuration and packaging

These guidelines describe coding style, formatting, and implementation expectations for this repository.

---

## General Coding Principles

- Prioritize **readability at a glance** over everything else unless there is a performance bottleneck.
- Follow principles from [Bjarne's guide](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) and also [PEP 8](https://www.python.org/dev/peps/pep-0008/) (don't follow Python-specific guidelines, just the general ones).
- Make changes **easy to review** (good commit messages, clean diffs, frequent and finished commits).
- Use **modern C++23** features (maybe even C++26, if LLVM releases it soon enough).
- Match the existing codebase style unless there is a strong reason to improve it.
- Favor **explicit naming** for plugin parameters, UI controls, and DSP state. 
- Use [clang-format](https://clang.llvm.org/docs/ClangFormat.html) and [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) for consistent formatting and static analysis (if not using clang-format, check the "Formatting Style" section below).
- Keep UI code and DSP code separated by responsibility:
    - `GVerbProcessor` handles audio/parameter logic
    - `GVerbEditor` handles layout, visuals, and attachments

---

## Formatting Style (if not using clang-format)

### Indentation and Braces
- Use **4 spaces consistently** within a file.
- Keep opening braces below function definitions and control blocks.
- Prefer compact, readable block structure over deeply nested logic.

### Spacing
- Follow the project’s current spacing.
- Keep alignment consistent within initializer lists and parameter blocks.

### Line Length
- Prefer short, readable lines.
- Break long initializer lists, function calls, and lambdas across lines when they become hard to scan.
- Keep comment lines brief and meaningful.

---

## C++ Style Rules

### Types and Declarations
- Prioritize **left-to-right readability** in variable declarations.
  - Prefer explicit types in all cases (e.g., int, float, bool, juce::String, vector<float>). 
  - Only use auto (or auto* / auto&) for long or nested types (e.g., `std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>`).
- Use `const` where possible for values that should not change.
- Use `constexpr` for compile-time constants.
- Prefer strongly typed JUCE types and clear parameter names.

### Initialization
- Prefer brace initialization where it improves safety or readability.
- Use member initializer lists for constructors.
- Keep parameter and attachment initialization in the order of member declaration.

### Lambdas and Structured Bindings
- Use lambdas for small inline transforms or callbacks but prefer functions whenever possible.
- Use structured bindings.
- Avoid over-compressing logic.

### Names
- Use descriptive, domain-relevant names (camelCase variables and functions, PascalCase types).

Examples of good names:
- `roomSizeSlider`
- `wetLevelAttachment`
- `syncReverbParams()`
- `createParameterLayout()`
- `GVerbProcessor`

### Comments
- Use comments to explain **why** (you can restate obvious code if it helps you in the long run).
- ~~If a block is self-explanatory, prefer clean naming over comments~~ Make code readable at a glance, even if it means adding a few comments.

---

## JUCE / Plugin Development Guidelines

### UI Code
- Attachments should be created in the editor constructor and should match APVTS parameter IDs exactly.
- Use helper structs or braced-init loops.

### DSP Code
- Avoid doing expensive work in the audio thread.
- Parameter smoothing should remain lightweight and predictable.
- Handle mono and stereo paths explicitly.

### State Management
- Keep parameter IDs stable once published.
- If a parameter name changes, check whether the ID should remain unchanged for compatibility.

### Thread Safety
- Keep `processBlock()` lightweight: no allocations, no locks, no heavy computations.
- Use atomics and smoothing for parameter updates.
- Avoid direct UI-to-audio dependencies.
- W JUCE for handling some thread safety.

### APVTS and Parameters
- Parameter IDs should remain lowercase and stable, such as:
    - `roomSize`
    - `damping`
    - `wetLevel`
    - `width`
    - `dryLevel`
- Display names may be more human-friendly than IDs.
- Default values should remain documented and consistent with the UI.

---

## File-Specific Expectations

Maintain Processor/UI separation rules defined above.
- `GVerbProcessor.hpp` should contain main DSP code.
- `GVerbEditor.hpp` should contain main UI code.
- `main.cpp` should contain only the plugin entry point registration.

### `CMakeLists.txt`
- Keep build flags and plugin format declarations readable.
- Preserve packaging logic for macOS targets.
- Avoid unnecessary build-system complexity unless it supports portability or maintainability.

---

## UI Guidelines

This plugin appears to target beginner-friendly producers, so UI labels and tooltips should:
- describe audible or musical effect, not implementation.
- avoid DSP or algorithm terminology unless necessary for clarity.

Good tooltip style:
- “Makes the reverb feel bigger or smaller.”
- “Controls how wide the reverb sounds in stereo.”
- “Sets how much reverb you hear in the mix.”

---

## Asset and Resource Handling

- Keep embedded assets in `assets/` and compile them through the existing binary data setup.
- Prefer using generated binary data helpers rather than loading external runtime files.
- Avoid introducing external runtime dependencies unless they are necessary.

---

## Review Checklist for Changes

Before submitting a change, check that:
- code matches the existing style
- no unnecessary hack was introduced
- comments are accurate and helpful
- everything still works as expected
- the plugin loads in major DAWs (Logic Pro, Ableton Live, Reaper, Reason, etc.)

---

## Preferred Implementation Approach

When adding or refactoring code in this project:
1. Improve readability first.
2. Preserve behavior second.
3. Don't break existing functionality without a good reason.

---

## When in Doubt

If a change affects:
- audio processing
- preset/state compatibility
- plugin packaging
- thread safety
- parameter ID stability

then prefer the safest, most ~~backwards-compatible~~ readable approach.

---

## Using AI

I honestly don't care if you (non-existent future contributor) use AI to write code, as long as it follows these guidelines and produces clean, maintainable code (please use Claude or GitHub Copilot). 
Make sure to review and test any AI-generated code carefully before committing it.

## Final Note

I may occasionally deviate from these rules, but the intent is to converge toward them. These guidelines strongly prioritize readability but are informed by real-world complexity in C++ codebases.
