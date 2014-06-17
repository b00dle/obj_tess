#include "BarkTexGenerator.h"


BarkTexGenerator::BarkTexGenerator(void):
	_bark(nullptr)
{
}

BarkTexGenerator::BarkTexGenerator(Bark* bark):
	_bark(new Bark( *bark ))
{
}

BarkTexGenerator::~BarkTexGenerator(void) {
}
