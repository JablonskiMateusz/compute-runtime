/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/execution_environment/execution_environment.h"
#include "runtime/memory_manager/os_agnostic_memory_manager.h"
#include "unit_tests/mocks/mock_memory_manager.h"

#include "test.h"
#include "gtest/gtest.h"

using namespace OCLRT;
class MemoryManagerGetAlloctionDataTest : public testing::TestWithParam<GraphicsAllocation::AllocationType> {
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST(MemoryManagerGetAlloctionDataTest, givenHostMemoryAllocationTypeAndAllocateMemoryFlagAndNullptrWhenAllocationDataIsQueriedThenCorrectFlagsAndSizeAreSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 10);
    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY);

    EXPECT_TRUE(allocData.flags.mustBeZeroCopy);
    EXPECT_TRUE(allocData.flags.useSystemMemory);
    EXPECT_EQ(10u, allocData.size);
    EXPECT_EQ(nullptr, allocData.hostPtr);
}

TEST(MemoryManagerGetAlloctionDataTest, givenNonHostMemoryAllocatoinTypeWhenAllocationDataIsQueriedThenMustBeZeroCopyAndUseSystemMemoryFlagsAreNotSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);

    EXPECT_FALSE(allocData.flags.mustBeZeroCopy);
    EXPECT_FALSE(allocData.flags.useSystemMemory);
    EXPECT_EQ(10u, allocData.size);
    EXPECT_EQ(nullptr, allocData.hostPtr);
}

TEST(MemoryManagerGetAlloctionDataTest, givenAllocateMemoryFlagTrueWhenHostPtrIsNotNullThenAllocationDataHasHostPtrNulled) {
    AllocationData allocData;
    char memory = 0;
    AllocationProperties properties(true, sizeof(memory));

    MockMemoryManager::getAllocationData(allocData, properties, 0, &memory, GraphicsAllocation::AllocationType::BUFFER);

    EXPECT_EQ(sizeof(memory), allocData.size);
    EXPECT_EQ(nullptr, allocData.hostPtr);
}

TEST(MemoryManagerGetAlloctionDataTest, givenBufferTypeWhenAllocationDataIsQueriedThenForcePinFlagIsSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);

    EXPECT_TRUE(allocData.flags.forcePin);
}

TEST(MemoryManagerGetAlloctionDataTest, givenBufferHostMemoryTypeWhenAllocationDataIsQueriedThenForcePinFlagIsSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY);

    EXPECT_TRUE(allocData.flags.forcePin);
}

TEST(MemoryManagerGetAlloctionDataTest, givenBufferCompressedTypeWhenAllocationDataIsQueriedThenForcePinFlagIsSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER_COMPRESSED);

    EXPECT_TRUE(allocData.flags.forcePin);
}

TEST(MemoryManagerGetAlloctionDataTest, givenDefaultAllocationFlagsWhenAllocationDataIsQueriedThenAllocateMemoryIsFalse) {
    AllocationData allocData;
    AllocationProperties properties;
    char memory;
    MockMemoryManager::getAllocationData(allocData, properties, 0, &memory, GraphicsAllocation::AllocationType::BUFFER_COMPRESSED);

    EXPECT_FALSE(allocData.flags.allocateMemory);
}

TEST(MemoryManagerGetAlloctionDataTest, givenSpecificDeviceWhenAllocationDataIsQueriedThenDeviceIsPropagatedToAllocationData) {
    AllocationData allocData;
    AllocationProperties properties(true, 0);
    MockMemoryManager::getAllocationData(allocData, properties, 3u, nullptr, GraphicsAllocation::AllocationType::BUFFER_COMPRESSED);

    EXPECT_EQ(3u, allocData.devicesBitfield);
}

typedef MemoryManagerGetAlloctionDataTest MemoryManagerGetAlloctionData32BitAnd64kbPagesAllowedTest;

TEST_P(MemoryManagerGetAlloctionData32BitAnd64kbPagesAllowedTest, givenAllocationTypesWith32BitAnd64kbPagesAllowedWhenAllocationDataIsQueriedThenProperFlagsAreSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 0);

    auto allocType = GetParam();
    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, allocType);

    EXPECT_TRUE(allocData.flags.allow32Bit);
    EXPECT_TRUE(allocData.flags.allow64kbPages);
    EXPECT_EQ(allocType, allocData.type);
}

TEST_P(MemoryManagerGetAlloctionData32BitAnd64kbPagesAllowedTest, given64kbAllowedAllocationTypeWhenAllocatingThenPreferRenderCompressionOnlyForSpecificTypes) {
    auto allocType = GetParam();
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, allocType);
    bool bufferCompressedType = (allocType == GraphicsAllocation::AllocationType::BUFFER_COMPRESSED);
    EXPECT_TRUE(allocData.flags.allow64kbPages);
    MockMemoryManager mockMemoryManager(true);
    auto allocation = mockMemoryManager.allocateGraphicsMemory(allocData);

    EXPECT_TRUE(mockMemoryManager.allocation64kbPageCreated);
    EXPECT_EQ(mockMemoryManager.preferRenderCompressedFlagPassed, bufferCompressedType);

    mockMemoryManager.freeGraphicsMemory(allocation);
}

typedef MemoryManagerGetAlloctionDataTest MemoryManagerGetAlloctionData32BitAnd64kbPagesNotAllowedTest;

TEST_P(MemoryManagerGetAlloctionData32BitAnd64kbPagesNotAllowedTest, givenAllocationTypesWith32BitAnd64kbPagesDisallowedWhenAllocationDataIsQueriedThenFlagsAreNotSet) {
    AllocationData allocData;
    AllocationProperties properties(true, 0);

    auto allocType = GetParam();
    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, allocType);

    EXPECT_FALSE(allocData.flags.allow32Bit);
    EXPECT_FALSE(allocData.flags.allow64kbPages);
    EXPECT_EQ(allocType, allocData.type);
}

static const GraphicsAllocation::AllocationType allocationTypesWith32BitAnd64KbPagesAllowed[] = {GraphicsAllocation::AllocationType::BUFFER,
                                                                                                 GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY,
                                                                                                 GraphicsAllocation::AllocationType::BUFFER_COMPRESSED,
                                                                                                 GraphicsAllocation::AllocationType::PIPE,
                                                                                                 GraphicsAllocation::AllocationType::SCRATCH_SURFACE,
                                                                                                 GraphicsAllocation::AllocationType::PRIVATE_SURFACE,
                                                                                                 GraphicsAllocation::AllocationType::PRINTF_SURFACE,
                                                                                                 GraphicsAllocation::AllocationType::CONSTANT_SURFACE,
                                                                                                 GraphicsAllocation::AllocationType::GLOBAL_SURFACE};

INSTANTIATE_TEST_CASE_P(Allow32BitAnd64kbPagesTypes,
                        MemoryManagerGetAlloctionData32BitAnd64kbPagesAllowedTest,
                        ::testing::ValuesIn(allocationTypesWith32BitAnd64KbPagesAllowed));

static const GraphicsAllocation::AllocationType allocationTypesWith32BitAnd64KbPagesNotAllowed[] = {GraphicsAllocation::AllocationType::COMMAND_BUFFER,
                                                                                                    GraphicsAllocation::AllocationType::DYNAMIC_STATE_HEAP,
                                                                                                    GraphicsAllocation::AllocationType::TIMESTAMP_TAG_BUFFER,
                                                                                                    GraphicsAllocation::AllocationType::IMAGE,
                                                                                                    GraphicsAllocation::AllocationType::INSTRUCTION_HEAP,
                                                                                                    GraphicsAllocation::AllocationType::SHARED_RESOURCE};

INSTANTIATE_TEST_CASE_P(Disallow32BitAnd64kbPagesTypes,
                        MemoryManagerGetAlloctionData32BitAnd64kbPagesNotAllowedTest,
                        ::testing::ValuesIn(allocationTypesWith32BitAnd64KbPagesNotAllowed));

TEST(MemoryManagerTest, givenForced32BitSetWhenGraphicsMemoryFor32BitAllowedTypeIsAllocatedThen32BitAllocationIsReturned) {
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, false, executionEnvironment);
    memoryManager.setForce32BitAllocations(true);

    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    if (is64bit) {
        EXPECT_TRUE(allocation->is32BitAllocation);
        EXPECT_EQ(MemoryPool::System4KBPagesWith32BitGpuAddressing, allocation->getMemoryPool());
    } else {
        EXPECT_FALSE(allocation->is32BitAllocation);
        EXPECT_EQ(MemoryPool::System4KBPages, allocation->getMemoryPool());
    }

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenForced32BitEnabledWhenGraphicsMemoryWihtoutAllow32BitFlagIsAllocatedThenNon32BitAllocationIsReturned) {
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, false, executionEnvironment);
    memoryManager.setForce32BitAllocations(true);

    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);
    allocData.flags.allow32Bit = false;

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    EXPECT_FALSE(allocation->is32BitAllocation);

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenForced32BitDisabledWhenGraphicsMemoryWith32BitFlagFor32BitAllowedTypeIsAllocatedThenNon32BitAllocationIsReturned) {
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, false, executionEnvironment);
    memoryManager.setForce32BitAllocations(false);

    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    EXPECT_FALSE(allocation->is32BitAllocation);

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenEnabled64kbPagesWhenGraphicsMemoryMustBeHostMemoryAndIsAllocatedWithNullptrForBufferThen64kbAllocationIsReturned) {
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(true, false, executionEnvironment);
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY);

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    EXPECT_EQ(0u, reinterpret_cast<uintptr_t>(allocation->getUnderlyingBuffer()) & MemoryConstants::page64kMask);
    EXPECT_EQ(0u, allocation->getGpuAddress() & MemoryConstants::page64kMask);
    EXPECT_EQ(0u, allocation->getUnderlyingBufferSize() & MemoryConstants::page64kMask);
    EXPECT_EQ(MemoryPool::System64KBPages, allocation->getMemoryPool());

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenEnabled64kbPagesWhenGraphicsMemoryWithoutAllow64kbPagesFlagsIsAllocatedThenNon64kbAllocationIsReturned) {
    MockMemoryManager memoryManager(true);
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);
    allocData.flags.allow64kbPages = false;

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    EXPECT_FALSE(memoryManager.allocation64kbPageCreated);
    EXPECT_TRUE(memoryManager.allocationCreated);

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenDisabled64kbPagesWhenGraphicsMemoryMustBeHostMemoryAndIsAllocatedWithNullptrForBufferThenNon64kbAllocationIsReturned) {
    MockMemoryManager memoryManager(false);
    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY);

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    EXPECT_FALSE(memoryManager.allocation64kbPageCreated);
    EXPECT_TRUE(memoryManager.allocationCreated);
    EXPECT_EQ(MemoryPool::System4KBPages, allocation->getMemoryPool());

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenForced32BitAndEnabled64kbPagesWhenGraphicsMemoryMustBeHostMemoryAndIsAllocatedWithNullptrForBufferThen32BitAllocationOver64kbIsChosen) {
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, false, executionEnvironment);
    memoryManager.setForce32BitAllocations(true);

    AllocationData allocData;
    AllocationProperties properties(true, 10);

    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY);

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    if (is64bit) {
        EXPECT_TRUE(allocation->is32BitAllocation);
    } else {
        EXPECT_FALSE(allocation->is32BitAllocation);
    }

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenEnabled64kbPagesWhenGraphicsMemoryIsAllocatedWithHostPtrForBufferThenExistingMemoryIsUsedForAllocation) {
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(true, false, executionEnvironment);
    AllocationData allocData;
    AllocationProperties properties(false, 1);

    char memory[1];
    MockMemoryManager::getAllocationData(allocData, properties, 0, &memory, GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY);

    auto allocation = memoryManager.allocateGraphicsMemory(allocData);
    ASSERT_NE(nullptr, allocation);
    EXPECT_EQ(1u, allocation->fragmentsStorage.fragmentCount);
    EXPECT_EQ(MemoryPool::System4KBPages, allocation->getMemoryPool());

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenMemoryManagerWhenGraphicsMemoryAllocationInDevicePoolFailsThenFallbackAllocationIsReturned) {
    MockMemoryManager memoryManager(false);

    memoryManager.failInDevicePool = true;

    auto allocation = memoryManager.allocateGraphicsMemoryInPreferredPool(AllocationProperties(true, MemoryConstants::pageSize), 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);
    ASSERT_NE(nullptr, allocation);
    EXPECT_TRUE(memoryManager.allocationCreated);
    EXPECT_EQ(MemoryPool::System4KBPages, allocation->getMemoryPool());

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenMemoryManagerWhenBufferTypeIsPassedThenAllocateGraphicsMemoryInPreferredPoolCanAllocateInDevicePool) {
    MockMemoryManager memoryManager(false);

    auto allocation = memoryManager.allocateGraphicsMemoryInPreferredPool(AllocationProperties(true, MemoryConstants::pageSize), 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);
    EXPECT_NE(nullptr, allocation);
    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenMemoryManagerWhenBufferTypeIsPassedAndAllocateInDevicePoolFailsWithErrorThenAllocateGraphicsMemoryInPreferredPoolReturnsNullptr) {
    MockMemoryManager memoryManager(false);

    memoryManager.failInDevicePoolWithError = true;

    auto allocation = memoryManager.allocateGraphicsMemoryInPreferredPool(AllocationProperties(true, MemoryConstants::pageSize), 0, nullptr, GraphicsAllocation::AllocationType::BUFFER);
    ASSERT_EQ(nullptr, allocation);
    EXPECT_FALSE(memoryManager.allocationInDevicePoolCreated);

    memoryManager.freeGraphicsMemory(allocation);
}

TEST(MemoryManagerTest, givenSvmAllocationTypeWhenGetAllocationDataIsCalledThenZeroCopyIsRequested) {
    MockMemoryManager memoryManager(false);
    AllocationData allocData;
    AllocationProperties properties(true, 1);
    MockMemoryManager::getAllocationData(allocData, properties, 0, nullptr, GraphicsAllocation::AllocationType::SVM);
    EXPECT_TRUE(allocData.flags.mustBeZeroCopy);
    EXPECT_TRUE(allocData.flags.allocateMemory);
}
