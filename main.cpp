#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "BinaryData.h"
// #include <fjstringelsienumbergdcodeobviouslyi'mahumancodeforcesisbetterthanusaco>

/*
  ==============================================================================

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
	DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	IN THE SOFTWARE.

  ==============================================================================
*/

class GVerbProcessor; // Plugin Backend Class
class GVerbEditor; // GUI Interface Class
static juce::Font getCustomFontTitle(); // Returns Custom Font for Title

class GVerbEditor : public juce::AudioProcessorEditor
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

	// Lays out coordinate map for GUI elements
	void resized() override
	{
		auto area = getLocalBounds();
		area.removeFromTop(40);
		auto leftArea = area.removeFromLeft(area.getWidth() / 2 + 50).reduced(20);
		leftArea.removeFromTop(30);
		leftArea.removeFromBottom(30);
		auto rightArea = area;
		rightArea.removeFromTop(70);

		const auto abletonOrange = juce::Colour(0xffff8800);
		const auto trackGrey = juce::Colour(0xff3a3a3a);
		const auto bgGrey = juce::Colour(0xff222222);
		const auto textGrey = juce::Colour(0xffaaaaaa);

		for (auto* slider : { &roomSizeSlider, &dampingSlider, &widthLevelSlider })
		{
			slider->setColour(juce::Slider::trackColourId, abletonOrange);
			slider->setColour(juce::Slider::thumbColourId, abletonOrange);
			slider->setColour(juce::Slider::backgroundColourId, bgGrey);
			slider->setColour(juce::Slider::textBoxTextColourId, textGrey);
			slider->setColour(juce::Slider::textBoxOutlineColourId, trackGrey);
		}

		for (auto* slider : { &wetLevelSlider, &dryLevelSlider })
		{
			slider->setColour(juce::Slider::rotarySliderFillColourId, abletonOrange);
			slider->setColour(juce::Slider::rotarySliderOutlineColourId, trackGrey);
			slider->setColour(juce::Slider::thumbColourId, abletonOrange);
			slider->setColour(juce::Slider::textBoxTextColourId, textGrey);
			slider->setColour(juce::Slider::textBoxOutlineColourId, trackGrey);
		}

		for (auto* label : { &roomSizeLabel, &dampingLabel, &widthLevelLabel, &wetLevelLabel, &dryLevelLabel })
			label->setColour(juce::Label::textColourId, textGrey);

		roomSizeSlider.setBounds(leftArea.removeFromLeft(100));
		dampingSlider.setBounds(leftArea.removeFromLeft(100));
		widthLevelSlider.setBounds(leftArea.removeFromLeft(100));

		wetLevelSlider.setBounds(rightArea.removeFromTop(100).withSizeKeepingCentre(140, 140));
		dryLevelSlider.setBounds(rightArea.removeFromTop(250).withSizeKeepingCentre(140, 140));
	}

	// Paints extra graphics
	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colour(0xff1a1a1a));

		g.setColour(juce::Colour(0xcc030307));
		g.fillRect(getLocalBounds());

		g.setColour(juce::Colour(0xff3a3a3a).withAlpha(0.5f));
		g.drawVerticalLine(static_cast<int>(static_cast<float>(getWidth()) * 0.6f), 90.0f, static_cast<float>(getHeight()) - 80);
		g.drawVerticalLine(static_cast<int>(static_cast<float>(getWidth()) * 0.6f - 1), 90.0f, static_cast<float>(getHeight()) - 80);

		g.setColour(juce::Colour(0xffff8800));
		g.setFont(getCustomFontTitle());
		g.drawText("G-Verb", getLocalBounds().removeFromTop(50), juce::Justification::centred, true);

		g.setColour(juce::Colour(0xffff8800).withAlpha(0.6f));
		g.fillRect(getLocalBounds().removeFromTop(52).removeFromBottom(3));

		g.setOpacity(.5f);
		g.drawImage(juce::ImageCache::getFromMemory(BinaryData::bottom_pic_png, BinaryData::bottom_pic_pngSize), getLocalBounds().removeFromBottom(10).toFloat());

		g.setOpacity(1.0f);
		g.setColour(juce::Colour(0xcc212129));
		g.drawRect(getLocalBounds(), 2);
	}

	bool supportsHostMIDIControllerPresence(bool hostMIDIControllerIsAvailable) override { return false; }
};

class GVerbProcessor : public juce::AudioProcessor
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

	void syncReverbParams()
	{
		// Receiving params from APVTS
		roomSize.setTargetValue(*roomSizeValue);
		damping.setTargetValue(*dampingValue);
		width.setTargetValue(*widthValue);
		wetLevel.setTargetValue(*wetLevelValue);
		dryLevel.setTargetValue(*dryLevelValue);

		// Setting params
		params.roomSize = roomSize.getNextValue();
		params.damping = damping.getNextValue();
		params.width = width.getNextValue();
		params.wetLevel = wetLevel.getNextValue();
		params.dryLevel = dryLevel.getNextValue();
		reverb.setParameters(params);
	}

	// Creates a list of parameters for APVTS to manage
	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
	{
		juce::AudioProcessorValueTreeState::ParameterLayout layout;

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			"roomSize", "Room Size",
			juce::NormalisableRange(0.0f, 1.0f), defaultRoomSize, juce::AudioParameterFloatAttributes().withValueFromStringFunction(
				[](const juce::String& s) { return s.getFloatValue(); }).withStringFromValueFunction(
				[](const float v, int) { return juce::String(v, 2); })));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			"damping", "Damping",
			juce::NormalisableRange(0.0f, 1.0f), defaultDamping, juce::AudioParameterFloatAttributes().withValueFromStringFunction(
				[](const juce::String& s) { return s.getFloatValue(); }).withStringFromValueFunction(
				[](const float v, int) { return juce::String(v, 2); })));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			"width", "Width",
			juce::NormalisableRange(0.0f, 1.0f), defaultWidth, juce::AudioParameterFloatAttributes().withValueFromStringFunction(
				[](const juce::String& s) { return s.getFloatValue(); }).withStringFromValueFunction(
				[](const float v, int) { return juce::String(v, 2); })));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			"wetLevel", "Wet Level",
			juce::NormalisableRange(0.0f, 1.0f), defaultWetLevel, juce::AudioParameterFloatAttributes().withValueFromStringFunction(
				[](const juce::String& s) { return s.getFloatValue(); }).withStringFromValueFunction(
				[](const float v, int) { return juce::String(v, 2); })));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			"dryLevel", "Dry Level",
			juce::NormalisableRange(0.0f, 1.0f), defaultDryLevel, juce::AudioParameterFloatAttributes().withValueFromStringFunction(
				[](const juce::String& s) { return s.getFloatValue(); }).withStringFromValueFunction(
				[](const float v, int) { return juce::String(v, 2); })));

		return layout;
	}

public:
	juce::AudioProcessorValueTreeState treeState;
	~GVerbProcessor() override = default;

	// Initializes headless engine when playback starts
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		reverb.setSampleRate(sampleRate);

		roomSize.reset(sampleRate, 0.05);
		damping.reset(sampleRate, 0.05);
		wetLevel.reset(sampleRate, 0.05);
		width.reset(sampleRate, 0.05);
		dryLevel.reset(sampleRate, 0.05);

		syncReverbParams();
	}

	// Headless processing
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
	{
		syncReverbParams();
		if (buffer.getNumChannels() >= 2)
		{
			// Getting channels for processing
			float* left { buffer.getWritePointer(0) };
			float* right { buffer.getWritePointer(1) };

			// Adding reverb
			reverb.processStereo(left, right, buffer.getNumSamples());
		}
		else if (buffer.getNumChannels() == 1)
		{
			// Getting channels for processing
			float* mono { buffer.getWritePointer(0) };

			// Adding reverb
			reverb.processMono(mono, buffer.getNumSamples());
		}
	}

	// Clears reverb state after playback has stopped
	void releaseResources() override
	{
		reverb.reset();
	}

	// Returns GVerbEditor for GUI
	juce::AudioProcessorEditor* createEditor() override
	{
		return new GVerbEditor(*this);
	}

	// Miscellaneous overrides
	const juce::String getName() const override { return "GVerb"; }
	const juce::String getProgramName(int) override { return "GVerb"; }
	juce::StringArray getAlternateDisplayNames() const override { return { "VVerb", "Venomous Verb", "G-Verb", "GNP Verb", "Goutham's Reverb"}; }
	double getTailLengthSeconds() const override { return reverb.getParameters().roomSize * 5.0; }
	bool hasEditor() const override { return true; }

	// Provides information to DAW for setting state
	void getStateInformation(juce::MemoryBlock& dest_data) override
	{
		auto state = treeState.copyState();
		std::unique_ptr<juce::XmlElement> xml (state.createXml());
		copyXmlToBinary (*xml, dest_data);
	}

	// Gets state from DAW
	void setStateInformation(const void* data, int size) override
	{
		std::unique_ptr xmlState (getXmlFromBinary (data, size));
		if (xmlState != nullptr)
		{
			if (xmlState->hasTagName (treeState.state.getType()))
			{
				treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
			}
		}
	}

	// Required overrides
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	bool isMidiEffect() const override { return false; }
	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	void changeProgramName(int, const juce::String&) override {}

	// Constructs processor with bus layouts (idk what these do but they are required for the plugin to work) and initializes APVTS pointers
	GVerbProcessor() : AudioProcessor (BusesProperties().withInput("Input",
		juce::AudioChannelSet::stereo())
		.withOutput("Output", juce::AudioChannelSet::stereo())), treeState(*this, &undoManager,
			juce::Identifier("GVerb"), createParameterLayout())
	{
		// Points param holders to APVTS interface
		roomSizeValue = treeState.getRawParameterValue("roomSize");
		dampingValue = treeState.getRawParameterValue("damping");
		wetLevelValue = treeState.getRawParameterValue("wetLevel");
		widthValue = treeState.getRawParameterValue("width");
		dryLevelValue = treeState.getRawParameterValue("dryLevel");
	}
};

// Constructs GUI Interface
GVerbEditor::GVerbEditor(GVerbProcessor& p) : AudioProcessorEditor(p), processor(p),
roomSizeAttachment(p.treeState, "roomSize", roomSizeSlider),
dampingAttachment(p.treeState, "damping", dampingSlider),
wetLevelAttachment(p.treeState, "wetLevel", wetLevelSlider),
widthLevelAttachment(p.treeState, "width", widthLevelSlider),
dryLevelAttachment(p.treeState, "dryLevel", dryLevelSlider)
{
	// Adds elements to logical GUI interface
	addAndMakeVisible(roomSizeSlider);
	addAndMakeVisible (roomSizeLabel);
	roomSizeLabel.setText ("Room Size", juce::dontSendNotification);
	roomSizeLabel.attachToComponent (&roomSizeSlider, false);
	roomSizeSlider.setSliderStyle(juce::Slider::LinearVertical);
	roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	roomSizeLabel.setJustificationType(juce::Justification::centred);
	roomSizeSlider.setNumDecimalPlacesToDisplay(2);
	roomSizeSlider.setTooltip("Size of the reverb impulse response");

	addAndMakeVisible(dampingSlider);
	addAndMakeVisible (dampingLabel);
	dampingLabel.setText ("Damping", juce::dontSendNotification);
	dampingLabel.attachToComponent (&dampingSlider, false);
	dampingSlider.setSliderStyle(juce::Slider::LinearVertical);
	dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	dampingLabel.setJustificationType(juce::Justification::centred);
	dampingSlider.setNumDecimalPlacesToDisplay(2);
	dampingSlider.setTooltip("Damping of the reverb impulse response");

	addAndMakeVisible(widthLevelSlider);
	addAndMakeVisible (widthLevelLabel);
	widthLevelLabel.setText ("Width", juce::dontSendNotification);
	widthLevelLabel.attachToComponent (&widthLevelSlider, false);
	widthLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
	widthLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	widthLevelLabel.setJustificationType(juce::Justification::centred);
	widthLevelSlider.setNumDecimalPlacesToDisplay(2);
	widthLevelSlider.setTooltip("Width of the reverb impulse response");

	addAndMakeVisible(wetLevelSlider);
	addAndMakeVisible (wetLevelLabel);
	wetLevelLabel.setText ("Wet level", juce::dontSendNotification);
	wetLevelLabel.attachToComponent (&wetLevelSlider, false);
	wetLevelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	wetLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	wetLevelLabel.setJustificationType(juce::Justification::centred);
	wetLevelSlider.setNumDecimalPlacesToDisplay(2);
	wetLevelSlider.setTooltip("Wet level of the reverb impulse response");

	addAndMakeVisible(dryLevelSlider);
	addAndMakeVisible (dryLevelLabel);
	dryLevelLabel.setText ("Dry level", juce::dontSendNotification);
	dryLevelLabel.attachToComponent (&dryLevelSlider, false);
	dryLevelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	dryLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	dryLevelLabel.setJustificationType(juce::Justification::centred);
	dryLevelSlider.setNumDecimalPlacesToDisplay(2);
	dryLevelSlider.setTooltip("Dry level of the reverb impulse response");

	// Sets GUI size and properties
	setSize(600, 450);
	setResizable(true, false);
	setOpaque(false);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new GVerbProcessor();
}

static juce::Font getCustomFontTitle()
{
	static auto typeface = juce::Typeface::createSystemTypefaceFor(
		BinaryData::Neuropol_otf,
		BinaryData::Neuropol_otfSize
	);

	return juce::FontOptions(typeface).withHeight(30.0f);
}
