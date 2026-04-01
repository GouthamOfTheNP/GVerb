#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class GVerbEditor; // Forward declaration to avoid circular dependency

class GVerbProcessor : public juce::AudioProcessor, public juce::ValueTree::Listener // Plugin Backend Class
{
private:
	// APVTS and processBlock values
	juce::UndoManager undoManager { 30000, 30 };
	juce::Reverb reverb;
	juce::Reverb::Parameters params;

	// Default initializations
	static constexpr float defaultRoomSize { 0.5f };
	static constexpr float defaultDamping { 0.3f };
	static constexpr float defaultWidth { 1.0f };
	static constexpr float defaultWetLevel { 0.3f };
	static constexpr float defaultDryLevel { 1.0f };

	// Smoothing for params
	juce::SmoothedValue<float> roomSize { defaultRoomSize };
	juce::SmoothedValue<float> damping { defaultDamping };
	juce::SmoothedValue<float> width { defaultWidth };
	juce::SmoothedValue<float> wetLevel { defaultWetLevel };
	juce::SmoothedValue<float> dryLevel { defaultDryLevel };

	// Thread-safe pointers to values from APVTS (initialized in constructor)
	std::atomic<float>* roomSizeValue = nullptr;
	std::atomic<float>* dampingValue = nullptr;
	std::atomic<float>* widthValue = nullptr;
	std::atomic<float>* wetLevelValue = nullptr;
	std::atomic<float>* dryLevelValue = nullptr;

	std::atomic<bool> paramsUpdated { true };

	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void syncReverbParams();


public:
	juce::AudioProcessorValueTreeState treeState;

	// Constructs processor with bus layouts (IDK what these do, but they are required for the plugin to work) and initializes APVTS pointers
	GVerbProcessor();
	~GVerbProcessor() override = default;

	// Initializes the headless engine when playback starts
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;

	// Headless processing
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

	// Clears reverb state after playback has stopped
	void releaseResources() override;

	// Provides information to DAW for setting state
	void getStateInformation(juce::MemoryBlock& dest_data) override;

	// Gets state from DAW
	void setStateInformation(const void* data, int size) override;

	// Returns GVerbEditor for GUI
	juce::AudioProcessorEditor* createEditor() override;

	// Miscellaneous overrides
	const juce::String getName() const override { return "GVerb"; }
	const juce::String getProgramName(int) override { return "GVerb"; }
	juce::StringArray getAlternateDisplayNames() const override { return { "VVerb", "Venomous Verb", "G-Verb", "GNP Verb", "Goutham's Reverb"}; }
	double getTailLengthSeconds() const override { return reverb.getParameters().roomSize * 5.0; }
	bool hasEditor() const override { return true; }
	void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

	// Required overrides
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	bool isMidiEffect() const override { return false; }
	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	void changeProgramName(int, const juce::String&) override {}
};
