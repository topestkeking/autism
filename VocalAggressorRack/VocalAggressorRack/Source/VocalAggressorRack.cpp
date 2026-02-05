#include "VocalAggressorRack.h"
#include "VocalAggressorRackEditor.h"

juce::AudioProcessorEditor* VocalAggressorRack::createEditor()
{
    return new VocalAggressorRackEditor (*this);
}
