#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "GVerbProcessor.hpp"

class GVerbEditor : public juce::AudioProcessorEditor // GUI Interface Class
{
private:
	GVerbProcessor& processor;

	// GUI Elements
	juce::Slider roomSizeSlider;
	juce::Label roomSizeLabel;
	juce::Slider dampingSlider;
	juce::Label dampingLabel;
	juce::Slider wetLevelSlider;
	juce::Label wetLevelLabel;
	juce::Slider widthLevelSlider;
	juce::Label widthLevelLabel;
	juce::Slider dryLevelSlider;
	juce::Label dryLevelLabel;

	// APVTS Communication Attachments
	juce::AudioProcessorValueTreeState::SliderAttachment roomSizeAttachment;
	juce::AudioProcessorValueTreeState::SliderAttachment dampingAttachment;
	juce::AudioProcessorValueTreeState::SliderAttachment wetLevelAttachment;
	juce::AudioProcessorValueTreeState::SliderAttachment widthLevelAttachment;
	juce::AudioProcessorValueTreeState::SliderAttachment dryLevelAttachment;

public:
	GVerbEditor(GVerbProcessor& p);

	// Lays out the coordinate map for GUI elements
	void resized() override;

	// Paints extra graphics
	void paint(juce::Graphics& g) override;

	bool supportsHostMIDIControllerPresence(bool hostMIDIControllerIsAvailable) override { return false; }
};

