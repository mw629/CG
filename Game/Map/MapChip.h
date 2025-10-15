#pragma once  
#include <Engine.h>  
#include "MapChipField.h"

class MapChipField;

class MapChip  
{  
private:  

    std::vector<std::vector<Model*>> models_;
    std::vector<std::vector<Transform>> tranform_;

    MapChipField* mapChipField_;


    const float blockSize_ = 1.0f;

  


public:  

    MapChip();
    ~MapChip();

    void Initialize(MapChipField * mapChipField, Matrix4x4 viewMatrix);

    void Update(Matrix4x4 viewMatrix);

    void Draw();

    void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };


};
