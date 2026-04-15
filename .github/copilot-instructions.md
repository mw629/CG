# Copilot Instructions

## プロジェクト ガイドライン
- The project changed its model management strategy to centralize loading. Instead of `CharacterAnimator` and `TransformAnimation` loading model files internally using `AssimpLoadObjFile`, they now expect a `ModelData` object to be passed into their `Initialize` methods, similar to `Model`.