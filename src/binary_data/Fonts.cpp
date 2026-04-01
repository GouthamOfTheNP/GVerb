#include "Fonts.hpp"
#include "BinaryData.h"

juce::Font getCustomFontTitle()
{
	static auto typeface = juce::Typeface::createSystemTypefaceFor(
		BinaryData::Neuropol_otf,
		BinaryData::Neuropol_otfSize
	);

	return juce::FontOptions(typeface).withHeight(30.0f);
}
