/* opencltest.c
 *
 * Copyright 2021 Hector Espert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS 1
#define CL_TARGET_OPENCL_VERSION 110


#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

int
main (int argc,
      char *argv[])
{
  printf ("Testing OpenCL\n");

  /* Get Platform and Device Info */
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_platforms;
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to get platform ids");
    return EXIT_FAILURE;
  }
  printf("Number of platforms found: %d\n", ret_num_platforms);

  cl_uint ret_num_devices;
  cl_device_id device_id = NULL;
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to get device ids");
    return EXIT_FAILURE;
  }

  printf("Number of devices found: %d\n", ret_num_devices);

  /* Create OpenCL context */
  cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to create context");
    return EXIT_FAILURE;
  }

  printf("Context created\n");

  /* Create Command Queue */
  cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to create command queue");
    return EXIT_FAILURE;
  }

  printf("Command queue created\n");

  FILE *fp;
  fp = fopen("hello.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to read kernel file.\n");
    return EXIT_FAILURE;
  }

  fseek(fp, 0, SEEK_END);

  size_t source_size = ftell(fp);

  rewind(fp);

  char *source_str = (char*) malloc(source_size + 1);

  source_size = fread(source_str, sizeof(char), source_size, fp);

  fclose(fp);

  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, NULL, &ret);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to create program from source");
    return EXIT_FAILURE;
  }

  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

  if (ret == CL_BUILD_PROGRAM_FAILURE) {
    size_t log_size;
    ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    if (ret != CL_SUCCESS)
    {
      fprintf(stderr, "Unable to get program build log size: %d\n", ret);
      return EXIT_FAILURE;
    }

    char *log = (char *) malloc(log_size);
    ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
    if (ret != CL_SUCCESS)
    {
      fprintf(stderr, "Unable to get program build log: %d\n", ret);
      return EXIT_FAILURE;
    }
    fprintf(stderr, "Build log: %s\n", log);
    return EXIT_FAILURE;
  }
  else if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to build program: %d\n", ret);
    return EXIT_FAILURE;
  }

  printf("Program build\n");

  cl_kernel kernel = clCreateKernel(program, "hello", &ret);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to create kernel");
    return EXIT_FAILURE;
  }

  printf("Kernel created\n");

  cl_mem memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 128 * sizeof(char), NULL, &ret);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to create buffer");
    return EXIT_FAILURE;
  }

  printf("Buffer created\n");

  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to set kernel arguments");
    return EXIT_FAILURE;
  }

  ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to enqueue task");
    return EXIT_FAILURE;
  }

  printf("Task enqueued\n");

  char string[128];
  ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, 128 * sizeof(char), string, 0, NULL, NULL);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to read queue buffer");
    return EXIT_FAILURE;
  }

  printf("Message from OPENCL: %s\n", string);

  ret = clFlush(command_queue);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to flush command queue");
    return EXIT_FAILURE;
  }


  ret = clFinish(command_queue);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to finish command queue");
    return EXIT_FAILURE;
  }


  ret = clReleaseMemObject(memobj);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to relase buffer");
    return EXIT_FAILURE;
  }

  printf("Buffer released\n");

  ret = clReleaseKernel(kernel);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to relase kernel");
    return EXIT_FAILURE;
  }

  printf("Kernel released\n");

  ret = clReleaseProgram(program);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to relase program");
    return EXIT_FAILURE;
  }

  printf("Program released\n");

  ret = clReleaseCommandQueue(command_queue);

  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to relase command queue");
    return EXIT_FAILURE;
  }

  printf("Command queue released\n");

  ret = clReleaseContext(context);
  if (ret != CL_SUCCESS)
  {
    fprintf(stderr, "Unable to relase context");
    return EXIT_FAILURE;
  }

  printf("Context released\n");

  return EXIT_SUCCESS;
}
