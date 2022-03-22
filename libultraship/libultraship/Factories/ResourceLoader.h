#include "../Resource.h"
#include "../File.h"

namespace Ship
{
    class ResourceLoader
    {
    public:
        static Resource* LoadResource(std::shared_ptr<File> FileToLoad);
    };
}