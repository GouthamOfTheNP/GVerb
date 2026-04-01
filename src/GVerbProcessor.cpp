#include "GVerbProcessor.hpp"
#include "GVerbEditor.hpp"

GVerbProcessor::GVerbProcessor() : AudioProcessor (BusesProperties().withInput("Input",
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
	treeState.state.addListener(this);
}

void GVerbProcessor::prepareToPlay(const double sampleRate, int samplesPerBlock)
{
	reverb.setSampleRate(sampleRate);

	roomSize.reset(sampleRate, 0.05);
	damping.reset(sampleRate, 0.05);
	wetLevel.reset(sampleRate, 0.05);
	width.reset(sampleRate, 0.05);
	dryLevel.reset(sampleRate, 0.05);

	syncReverbParams(samplesPerBlock);
}

void GVerbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	if (paramsUpdated.exchange(false))
	{
		syncReverbParams(buffer.getNumSamples());
	}

	if (buffer.getNumChannels() >= 2)
	{
		float* left_channel_pointer { buffer.getWritePointer(0) };
		float* right_channel_pointer { buffer.getWritePointer(1) };
		reverb.processStereo(left_channel_pointer, right_channel_pointer, buffer.getNumSamples());
	}
	else if (buffer.getNumChannels() == 1)
	{
		float* mono_channel_pointer { buffer.getWritePointer(0) };
		reverb.processMono(mono_channel_pointer, buffer.getNumSamples());
	}
}

void GVerbProcessor::releaseResources()
{
	reverb.reset();
}

void GVerbProcessor::syncReverbParams(int numSamples)
{
	// Receiving params from APVTS
	roomSize.setTargetValue(*roomSizeValue);
	damping.setTargetValue(*dampingValue);
	width.setTargetValue(*widthValue);
	wetLevel.setTargetValue(*wetLevelValue);
	dryLevel.setTargetValue(*dryLevelValue);

	// Setting params
	params.roomSize = roomSize.skip(numSamples);
	params.damping = damping.skip(numSamples);
	params.width = width.skip(numSamples);
	params.wetLevel = wetLevel.skip(numSamples);
	params.dryLevel = dryLevel.skip(numSamples);

	reverb.setParameters(params);
}

void GVerbProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	const auto state = treeState.copyState();
	const std::unique_ptr xml (state.createXml());
	copyXmlToBinary(*xml, destData);
}

void GVerbProcessor::setStateInformation(const void* data, const int size)
{
	if (const std::unique_ptr xmlState (getXmlFromBinary (data, size)); xmlState != nullptr)
	{
		if (xmlState->hasTagName (treeState.state.getType()))
		{
			treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
		}
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout GVerbProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    struct ParamSpec
    {
        const char* id;
        const char* name;
        float defaultValue;
    };

	constexpr std::array<ParamSpec, 5> params {{
        { "roomSize", "Room Size", defaultRoomSize },
        { "damping", "Damping", defaultDamping },
        { "width", "Width", defaultWidth },
        { "wetLevel", "Wet Level", defaultWetLevel },
        { "dryLevel", "Dry Level", defaultDryLevel }
    }};

    for (const auto& [id, name, defaultValue] : params)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
        	id, name, juce::NormalisableRange(0.0f, 1.0f), defaultValue,
            juce::AudioParameterFloatAttributes().withValueFromStringFunction([](const juce::String& s)
            	{
                    return s.getFloatValue();
                })
                .withStringFromValueFunction([](const float v, int)
                {
                    return juce::String(v, 2);
                })));
    }

    return layout;
}

juce::AudioProcessorEditor* GVerbProcessor::createEditor()
{
	return new GVerbEditor(*this);
}

void GVerbProcessor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
	paramsUpdated.store(true);
}







