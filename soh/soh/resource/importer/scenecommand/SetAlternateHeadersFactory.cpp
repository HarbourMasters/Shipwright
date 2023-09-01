#include "soh/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"

namespace LUS {
std::shared_ptr<IResource> SetAlternateHeadersFactory::ReadResource(std::shared_ptr<ResourceInitData> initData,
                                                                   std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetAlternateHeaders>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetAlternateHeadersFactoryV0>();
	    break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load SetAlternateHeaders with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetAlternateHeadersFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource)
{
    std::shared_ptr<SetAlternateHeaders> setAlternateHeaders = std::static_pointer_cast<SetAlternateHeaders>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setAlternateHeaders);

    ReadCommandId(setAlternateHeaders, reader);
	
    setAlternateHeaders->numHeaders = reader->ReadUInt32();
    setAlternateHeaders->headers.reserve(setAlternateHeaders->numHeaders);
    for (uint32_t i = 0; i < setAlternateHeaders->numHeaders; i++) {
	auto headerName = reader->ReadString();
	if (!headerName.empty()) {
		setAlternateHeaders->headers.push_back(std::static_pointer_cast<LUS::Scene>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(headerName.c_str())));
	} else {
		setAlternateHeaders->headers.push_back(nullptr);
	}
    }
}

} // namespace LUS
