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
	// Adds elements to the logical GUI interface
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
	auto area = getLocalBounds();
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
