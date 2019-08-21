/*
 * Copyright (C) 2018-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/os_interface/windows/wddm_memory_operations_handler.h"
#include "test.h"
#include "unit_tests/mocks/mock_allocation_properties.h"
#include "unit_tests/mocks/mock_wddm.h"
#include "unit_tests/os_interface/windows/mock_wddm_allocation.h"
#include "unit_tests/os_interface/windows/wddm_fixture.h"

using namespace NEO;

struct WddmMemoryOperationsHandlerTest : public WddmTest {
    void SetUp() override {
        WddmTest::SetUp();
        wddmMemoryOperationsHandler = std::make_unique<WddmMemoryOperationsHandler>(wddm);
        wddmAllocation.handle = 0x2;
    }

    std::unique_ptr<WddmMemoryOperationsHandler> wddmMemoryOperationsHandler;
    MockWddmAllocation wddmAllocation;
};

TEST_F(WddmMemoryOperationsHandlerTest, whenMakingResidentAllocaionExpectMakeResidentCalled) {
    EXPECT_TRUE(wddmMemoryOperationsHandler->makeResident(wddmAllocation));
    EXPECT_TRUE(wddmMemoryOperationsHandler->isResident(wddmAllocation));
}

TEST_F(WddmMemoryOperationsHandlerTest, whenEvictingResidentAllocationExpectEvictCalled) {
    EXPECT_TRUE(wddmMemoryOperationsHandler->makeResident(wddmAllocation));
    EXPECT_TRUE(wddmMemoryOperationsHandler->evict(wddmAllocation));
    EXPECT_FALSE(wddmMemoryOperationsHandler->isResident(wddmAllocation));
}
