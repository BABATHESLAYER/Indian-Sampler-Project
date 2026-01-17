cmake_minimum_required(VERSION 3.22)

project(DesiSamplerPro VERSION 1.0.0)

# -----------------------------------------------------------------------------
# JUCE Setup
# -----------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)

# Enable MP3 Support as requested
set(JUCE_USE_MP3AUDIOFORMAT ON CACHE BOOL "Enable MP3 Audio Format" FORCE)

FetchContent_MakeAvailable(juce)

# -----------------------------------------------------------------------------
# Plugin / App Definition
# -----------------------------------------------------------------------------
juce_add_plugin(DesiSamplerPro
    COMPANY_NAME "JulesCode"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE JuCo
    PLUGIN_CODE Dsp1
    FORMATS VST3 Standalone
    PRODUCT_NAME "DesiSamplerPro"
)

# -----------------------------------------------------------------------------
# Source Files
# -----------------------------------------------------------------------------
target_sources(DesiSamplerPro PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/AudioEngine/DesiSamplerVoice.h
    Source/AudioEngine/DesiSamplerSound.h
    Source/UI/DesiLookAndFeel.h
    Source/UI/GlobalLED.h
    Source/UI/SampleBrowser.h
    Source/StandaloneBridge.h
)

# -----------------------------------------------------------------------------
# Standalone Specifics
# -----------------------------------------------------------------------------
target_sources(DesiSamplerPro_Standalone PRIVATE Source/StandaloneBridge.cpp)

# -----------------------------------------------------------------------------
# VST3 Specifics
# -----------------------------------------------------------------------------
target_sources(DesiSamplerPro_VST3 PRIVATE Source/StandaloneBridgeDummy.cpp)

# -----------------------------------------------------------------------------
# Link Libraries
# -----------------------------------------------------------------------------
target_link_libraries(DesiSamplerPro PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
    juce_recommended_config_flags
    juce_recommended_lto_flags
    juce_recommended_warning_flags
)

# -----------------------------------------------------------------------------
# Compile Definitions
# -----------------------------------------------------------------------------
target_compile_definitions(DesiSamplerPro PUBLIC
    JUCE_WEB_BROWSER=0  # Disable web browser to save build time/deps
    JUCE_USE_CURL=0     # Use native networking
)
