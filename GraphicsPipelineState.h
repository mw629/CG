#include "RootSignature.h"
#include "RootParameter.h"
#include "InputLayout.h"
#include "BlendState.h"


class GraphicsPipelineState {
private:
	RootSignature* rootSignature_;
	RootParameter* rootParameter_;
	InputLayout* inputLayout_;
	BlendState* blendState_;

public:
	~GraphicsPipelineState();

	void CreatePSO(std::ostream& os);
};