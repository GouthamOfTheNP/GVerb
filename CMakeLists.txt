cmake_minimum_required(VERSION 3.22)
project(SampleAnalyzer VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/JUCE)

juce_add_plugin(GVerb
        PRODUCT_NAME "GVerb"
        PLUGIN_MANUFACTURER_CODE GNPE
        PLUGIN_CODE GVrb
        FORMATS VST3 Standalone AU
        VST3_AUTO_MANIFEST FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_SYNTH FALSE
        DESCRIPTION "Factory reverb plugin"
        ICON_BIG ${CMAKE_SOURCE_DIR}/assets/icon_big.png
        ICON_SMALL ${CMAKE_SOURCE_DIR}/assets/icon_big.png
)

file(GLOB_RECURSE PLUGIN_ASSETS "assets/*.*")
juce_add_binary_data(PluginAssets
        NAMESPACE BinaryData
        SOURCES ${PLUGIN_ASSETS}
)

juce_generate_juce_header(GVerb)

target_sources(GVerb
        PRIVATE
        main.cpp
)

target_link_libraries(GVerb
        PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        PluginAssets
)

target_include_directories(GVerb
        SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}
        $<TARGET_PROPERTY:PluginAssets,INTERFACE_INCLUDE_DIRECTORIES>
)

add_custom_command(TARGET GVerb_Standalone POST_BUILD
        COMMAND xattr -cr "$<TARGET_BUNDLE_DIR:GVerb_Standalone>"
        COMMAND codesign --force --deep -s - "$<TARGET_BUNDLE_DIR:GVerb_Standalone>"
)

add_custom_command(TARGET GVerb_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf
        "${CMAKE_BINARY_DIR}/pkg_staging"
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_BINARY_DIR}/pkg_staging/Library/Audio/Plug-Ins/VST3"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_BUNDLE_DIR:GVerb_VST3>"
        "${CMAKE_BINARY_DIR}/pkg_staging/Library/Audio/Plug-Ins/VST3/GVerb.vst3"
        COMMAND codesign --force -s -
        "${CMAKE_BINARY_DIR}/pkg_staging/Library/Audio/Plug-Ins/VST3/GVerb.vst3"
        COMMAND pkgbuild
        --root "${CMAKE_BINARY_DIR}/pkg_staging"
        --install-location "/"
        --identifier "com.gnpe.gverb"
        --version "0.0.1"
        "${CMAKE_BINARY_DIR}/GVerb.pkg"
)

add_custom_command(TARGET GVerb_AU POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf
        "${CMAKE_BINARY_DIR}/au_staging"
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_BINARY_DIR}/au_staging/Library/Audio/Plug-Ins/Components"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_BUNDLE_DIR:GVerb_AU>"
        "${CMAKE_BINARY_DIR}/au_staging/Library/Audio/Plug-Ins/Components/GVerb.component"
        COMMAND codesign --force -s -
        "${CMAKE_BINARY_DIR}/au_staging/Library/Audio/Plug-Ins/Components/GVerb.component"
        COMMAND pkgbuild
        --root "${CMAKE_BINARY_DIR}/au_staging"
        --install-location "/"
        --identifier "com.gnpe.gverbau"
        --version "0.0.1"
        "${CMAKE_BINARY_DIR}/GVerbAU.pkg"
)

target_compile_definitions(GVerb
        PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)
