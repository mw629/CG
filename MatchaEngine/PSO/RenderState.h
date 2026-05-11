#pragma once

enum BlendMode {
	kBlendModeNone,
	kBlendModeNormal,
	kBlendModeAdd,
	kBlendModeSubtract,
	kBlendModeMultiply,
	kBlendModeScreen,

	kBlendNum,
};


// ShaderName was removed to use string IDs

static const char* BlendModeToString(BlendMode mode)
{
	switch (mode) {
	case kBlendModeNone: return "kBlendModeNone";
	case kBlendModeNormal: return "kBlendModeNormal";
	case kBlendModeAdd: return "kBlendModeAdd";
	case kBlendModeSubtract: return "kBlendModeSubtract";
	case kBlendModeMultiply: return "kBlendModeMultiply";
	case kBlendModeScreen: return "kBlendModeScreen";
	default: return "UnknownBlendMode";
	}
}
