#include "GVerbEditor.hpp"
#include "BinaryData.h"
#include "binary_data/Fonts.hpp"

GVerbEditor::GVerbEditor(GVerbProcessor& p) : AudioProcessorEditor(p), processor(p),
roomSizeAttachment(p.treeState, "roomSize", roomSizeSlider),
dampingAttachment(p.treeState, "damping", dampingSlider),
wetLevelAttachment(p.treeState, "wetLevel", wetLevelSlider),
widthLevelAttachment(p.treeState, "width", widthLevelSlider),
dryLevelAttachment(p.treeState, "dryLevel", dryLevelSlider)
{
	// Adds elements to the logical GUI interface using a struct of pointers, a braced-init loop, and structured bindings
	struct SliderSpec
	{
	    juce::Slider* slider;
	    juce::Label* label;
	    const char* text;
	    const char* tooltip;
	    juce::Slider::SliderStyle style;
	};

	for (const auto& [slider, label, text, tooltip, style] : {
		SliderSpec{
			&roomSizeSlider, &roomSizeLabel, "Room Size",
			"Makes the reverb feel like it's in a bigger or smaller space. Higher values usually sound more spacious and longer.",
			juce::Slider::LinearVertical
		},
		SliderSpec{
			&dampingSlider, &dampingLabel, "Damping",
			"Softens the bright, sharp parts of the reverb. Higher values make the reverb smoother and darker.",
			juce::Slider::LinearVertical
		},
		SliderSpec{
			&widthLevelSlider, &widthLevelLabel, "Width",
			"Controls how wide the reverb sounds in stereo. Higher values spread it out more; lower values keep it centered.",
			juce::Slider::LinearVertical
		},
		SliderSpec{
			&wetLevelSlider, &wetLevelLabel, "Wet level",
			"Sets how much reverb you hear in the mix. Turn it up for a bigger effect, or down for a cleaner, drier sound.",
			juce::Slider::RotaryHorizontalVerticalDrag
		},
		SliderSpec{
			&dryLevelSlider, &dryLevelLabel, "Dry level",
			"Sets how much of the original sound stays in the mix. Lower it if you want the reverb to stand out more.",
			juce::Slider::RotaryHorizontalVerticalDrag
		}
	})
	{
	    addAndMakeVisible(*slider);
	    addAndMakeVisible(*label);

	    label->setText(text, juce::dontSendNotification);
	    label->attachToComponent(slider, false);
	    label->setJustificationType(juce::Justification::centred);

	    slider->setSliderStyle(style);
	    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	    slider->setNumDecimalPlacesToDisplay(2);
	    slider->setTooltip(tooltip);
	}

	// Sets colors for sliders and knobs
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
	{
		label->setColour(juce::Label::textColourId, textGrey);
	}

	// Sets GUI size and properties
	setSize(600, 450);
	setResizable(true, false);
	setOpaque(false);
}

void GVerbEditor::resized()
{
	juce::Rectangle<int> area = getLocalBounds();
	area.removeFromTop(40);
	auto leftArea = area.removeFromLeft(area.getWidth() / 2 + 50).reduced(20);
	leftArea.removeFromTop(30);
	leftArea.removeFromBottom(30);
	auto rightArea = area;
	rightArea.removeFromTop(70);

	roomSizeSlider.setBounds(leftArea.removeFromLeft(100));
	dampingSlider.setBounds(leftArea.removeFromLeft(100));
	widthLevelSlider.setBounds(leftArea.removeFromLeft(100));

	wetLevelSlider.setBounds(rightArea.removeFromTop(100).withSizeKeepingCentre(140, 140));
	dryLevelSlider.setBounds(rightArea.removeFromTop(250).withSizeKeepingCentre(140, 140));
}

void GVerbEditor::paint(juce::Graphics& g)
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
