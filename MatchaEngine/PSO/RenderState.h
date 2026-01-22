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


enum ShaderName {
	ObjectShader,
	ParticleShader,
	LineShader,

	ShaderNum,
};


static const char* ShaderNameToString(ShaderName name)
{
	switch (name) {
	case ObjectShader:  return "ObjectShader";
	case ParticleShader: return "ParticleShader";
	case LineShader: return "LineShader";
	default: return "UnknownShader";
	}
}

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
