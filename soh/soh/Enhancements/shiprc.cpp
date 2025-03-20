#include <Context.h>
#include "soh/ShipInit.hpp"
#include <libultraship/libultraship.h>
#include <fstream>
#include <string>

void Shiprc() {
    std::shared_ptr<Ship::Context> context = Ship::Context::GetInstance();
    auto console = context->GetConsole();
    std::string path = context->GetPathRelativeToAppDirectory(".shiprc");
    std::string line;
    std::string output;

    std::fstream fileShiprc(path);
    if (fileShiprc.is_open()) {
        while (std::getline(fileShiprc, line)) {
            int32_t commandResult = console->Run(line, &output);
        }
    }
    fileShiprc.close();
}

static RegisterShipInitFunc initFunc(Shiprc, {});
