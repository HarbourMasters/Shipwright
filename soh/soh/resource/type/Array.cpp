#include "Array.h"
#include "graphic/Fast3D/lus_gbi.h"
namespace SOH {
Array::Array() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
}

void* Array::GetPointer() {
    void* dataPointer = nullptr;
    switch (ArrayType) {
        case ArrayResourceType::Vertex:
            dataPointer = Vertices.data();
            break;
        case ArrayResourceType::Scalar:
        default:
            dataPointer = Scalars.data();
            break;
    }

    return dataPointer;
}

size_t Array::GetPointerSize() {
    size_t typeSize = 0;
    switch (ArrayType) {
        case ArrayResourceType::Vertex:
            typeSize = sizeof(F3DVtx);
            break;
        case ArrayResourceType::Scalar:
        default:
            switch (ArrayScalarType) {
                case ScalarType::ZSCALAR_S16:
                    typeSize = sizeof(int16_t);
                    break;
                case ScalarType::ZSCALAR_U16:
                    typeSize = sizeof(uint16_t);
                    break;
                default:
                    // OTRTODO: IMPLEMENT OTHER TYPES!
                    break;
            }
            break;
    }
    return ArrayCount * typeSize;
}
} // namespace LUS
