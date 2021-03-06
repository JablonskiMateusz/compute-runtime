# Copyright (c) 2017 - 2018, Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

set(IGDRCL_SRCS_tests_command_queue
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/buffer_operations_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/buffer_operations_withAsyncGPU_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/command_queue_flush_waitlist_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/command_queue_hw_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/command_queue_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dispatch_walker_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/drm_requirements_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_api_tests_mt_with_asyncGPU.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_barrier_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_event_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_rect_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_rect_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_to_image_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_buffer_to_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_image_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_image_to_buffer_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_copy_image_to_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_debug_kernel_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fill_buffer_event_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fill_buffer_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fill_buffer_negative_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fill_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fill_image_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fill_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fixture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_handler_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_event_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_global_offset_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_local_work_size_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_two_ioq_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_two_ooq_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_two_walker_ioq_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_kernel_two_walker_ooq_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_map_buffer_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_map_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_map_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_marker_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_media_kernel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_migrate_mem_objects_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_buffer_event_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_buffer_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_buffer_rect_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_buffer_rect_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_image_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_read_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_svm_mem_copy_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_svm_mem_fill_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_svm_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_thread_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_unmap_memobject_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_waitlist_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_write_buffer_event_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_write_buffer_rect_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_write_buffer_rect_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_write_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_write_image_fixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enqueue_write_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/finish_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flattened_id_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flush_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/get_command_queue_info_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/get_size_required_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/get_size_required_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/get_size_required_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ioq_task_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ioq_task_tests_mt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/local_id_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/local_work_size_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/multi_dispatch_info_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/multiple_map_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/multiple_map_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/oom_buffer_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/oom_image_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/oom_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ooq_task_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ooq_task_tests_mt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/read_write_buffer_cpu_copy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/work_group_size_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/zero_size_enqueue_tests.cpp
)
target_sources(igdrcl_tests PRIVATE ${IGDRCL_SRCS_tests_command_queue})
add_subdirectories()
