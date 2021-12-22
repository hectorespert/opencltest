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
