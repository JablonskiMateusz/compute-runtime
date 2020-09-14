/*
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/memory_manager/unified_memory_manager.h"
#include "shared/test/unit_test/page_fault_manager/cpu_page_fault_manager_tests_fixture.h"
#include "shared/test/unit_test/test_macros/test_checks_shared.h"

#include "opencl/source/command_queue/command_queue.h"
#include "opencl/test/unit_test/mocks/mock_command_queue.h"
#include "opencl/test/unit_test/mocks/mock_graphics_allocation.h"
#include "opencl/test/unit_test/mocks/mock_memory_manager.h"

#include "gtest/gtest.h"

using namespace NEO;

struct CommandQueueMock : public MockCommandQueue {
    cl_int enqueueSVMUnmap(void *svmPtr,
                           cl_uint numEventsInWaitList, const cl_event *eventWaitList,
                           cl_event *event, bool externalAppCall) override {
        transferToGpuCalled++;
        return CL_SUCCESS;
    }
    cl_int enqueueSVMMap(cl_bool blockingMap, cl_map_flags mapFlags,
                         void *svmPtr, size_t size,
                         cl_uint numEventsInWaitList, const cl_event *eventWaitList,
                         cl_event *event, bool externalAppCall) override {
        transferToCpuCalled++;
        passedMapFlags = mapFlags;
        return CL_SUCCESS;
    }
    cl_int finish() override {
        finishCalled++;
        return CL_SUCCESS;
    }

    int transferToCpuCalled = 0;
    int transferToGpuCalled = 0;
    int finishCalled = 0;
    uint64_t passedMapFlags = 0;
};

TEST_F(PageFaultManagerTest, givenUnifiedMemoryAllocWhenSynchronizeMemoryThenEnqueueProperCalls) {
    MockExecutionEnvironment executionEnvironment;
    REQUIRE_SVM_OR_SKIP(executionEnvironment.rootDeviceEnvironments[0]->getHardwareInfo());

    auto memoryManager = std::make_unique<MockMemoryManager>(executionEnvironment);
    auto svmAllocsManager = std::make_unique<SVMAllocsManager>(memoryManager.get());
    void *alloc = svmAllocsManager->createSVMAlloc(mockRootDeviceIndex, 256, {}, mockDeviceBitfield);

    auto cmdQ = std::make_unique<CommandQueueMock>();
    pageFaultManager->insertAllocation(alloc, 256, svmAllocsManager.get(), cmdQ.get(), {});

    pageFaultManager->baseCpuTransfer(alloc, 10, cmdQ.get());
    EXPECT_EQ(cmdQ->transferToCpuCalled, 1);
    EXPECT_EQ(cmdQ->transferToGpuCalled, 0);
    EXPECT_EQ(cmdQ->finishCalled, 0);

    pageFaultManager->baseGpuTransfer(alloc, cmdQ.get());
    EXPECT_EQ(cmdQ->transferToCpuCalled, 1);
    EXPECT_EQ(cmdQ->transferToGpuCalled, 1);
    EXPECT_EQ(cmdQ->finishCalled, 1);

    svmAllocsManager->freeSVMAlloc(alloc);
}

TEST_F(PageFaultManagerTest, givenUnifiedMemoryAllocWhenGpuTransferIsInvokedThenInsertMapOperation) {
    MockExecutionEnvironment executionEnvironment;
    REQUIRE_SVM_OR_SKIP(executionEnvironment.rootDeviceEnvironments[0]->getHardwareInfo());

    struct MockSVMAllocsManager : SVMAllocsManager {
        using SVMAllocsManager::SVMAllocsManager;
        void insertSvmMapOperation(void *regionSvmPtr, size_t regionSize, void *baseSvmPtr, size_t offset, bool readOnlyMap) override {
            SVMAllocsManager::insertSvmMapOperation(regionSvmPtr, regionSize, baseSvmPtr, offset, readOnlyMap);
            insertSvmMapOperationCalled++;
        }
        int insertSvmMapOperationCalled = 0;
    };
    auto memoryManager = std::make_unique<MockMemoryManager>(executionEnvironment);
    auto svmAllocsManager = std::make_unique<MockSVMAllocsManager>(memoryManager.get());
    void *alloc = svmAllocsManager->createSVMAlloc(mockRootDeviceIndex, 256, {}, mockDeviceBitfield);
    auto cmdQ = std::make_unique<CommandQueueMock>();
    pageFaultManager->insertAllocation(alloc, 256, svmAllocsManager.get(), cmdQ.get(), {});

    EXPECT_EQ(svmAllocsManager->insertSvmMapOperationCalled, 0);
    pageFaultManager->baseGpuTransfer(alloc, cmdQ.get());
    EXPECT_EQ(svmAllocsManager->insertSvmMapOperationCalled, 1);

    svmAllocsManager->freeSVMAlloc(alloc);
}
