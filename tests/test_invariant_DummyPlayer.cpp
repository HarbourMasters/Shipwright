#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>
#include <vector>

// Include the actual header for DummyPlayer functionality
#include "soh/soh/Network/Anchor/DummyPlayer.h"

// We need to verify that function pointer fields in actor structures
// are never overwritten by network-received data.

// Sentinel values representing known-bad function pointer overwrites
static constexpr uintptr_t EXPLOIT_PAYLOAD_ADDR = 0x4141414141414141ULL;
static constexpr uintptr_t NULL_PTR_ADDR = 0x0000000000000000ULL;
static constexpr uintptr_t STACK_SPRAY_ADDR = 0xDEADBEEFDEADBEEFULL;

struct MockClientData {
    std::vector<uint8_t> raw;
};

class DummyPlayerSecurityTest : public ::testing::TestWithParam<uintptr_t> {};

TEST_P(DummyPlayerSecurityTest, FunctionPointersNotOverwrittenByNetworkData) {
    // Invariant: After applying network-received client data to a DummyPlayer actor,
    // function pointer / vtable fields must never contain attacker-controlled values.
    uintptr_t malicious_value = GetParam();

    // Create a DummyPlayer instance
    DummyPlayer dummy;
    memset(&dummy, 0, sizeof(dummy));

    // Store original function pointers (update, draw, destroy callbacks)
    auto original_update = dummy.actor.update;
    auto original_draw = dummy.actor.draw;
    auto original_destroy = dummy.actor.destroy;

    // Craft a malicious network payload that attempts to overwrite function pointers
    // by filling the entire client data buffer with the malicious value
    std::vector<uint8_t> payload(sizeof(Actor), 0);
    for (size_t i = 0; i < payload.size(); i += sizeof(uintptr_t)) {
        if (i + sizeof(uintptr_t) <= payload.size()) {
            memcpy(&payload[i], &malicious_value, sizeof(uintptr_t));
        }
    }

    // Apply the network data (simulating what UpdateFromClientData does)
    dummy.UpdateFromClientData(payload.data(), payload.size());

    // SECURITY INVARIANT: Function pointers must not be overwritten to attacker values
    ASSERT_NE((uintptr_t)dummy.actor.update, malicious_value)
        << "update function pointer was overwritten by network data";
    ASSERT_NE((uintptr_t)dummy.actor.draw, malicious_value)
        << "draw function pointer was overwritten by network data";
    ASSERT_NE((uintptr_t)dummy.actor.destroy, malicious_value)
        << "destroy function pointer was overwritten by network data";
}

INSTANTIATE_TEST_SUITE_P(
    AdversarialInputs,
    DummyPlayerSecurityTest,
    ::testing::Values(
        EXPLOIT_PAYLOAD_ADDR,
        NULL_PTR_ADDR,
        STACK_SPRAY_ADDR
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}