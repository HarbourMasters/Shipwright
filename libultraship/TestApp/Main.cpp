#include "OTRArchive.h"

void SaveTest()
{
	auto archPtr = OtrLib::OTRArchive::CreateArchive("myarch.mpq");
	auto arch = archPtr.get();

	char data[] = "ABCDEFG!";

	arch->AddFile("test.txt", (uintptr_t)data, sizeof(data));
}

void LoadTest()
{
	std::shared_ptr<OtrLib::OTRArchive> archPtr(new OtrLib::OTRArchive("myarch.mpq"));

	archPtr->LoadFile("(crcfile)");

	int bp = 0;

}

int main()
{
	LoadTest();
	//SaveTest();

	return 0;
}