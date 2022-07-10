/*
 * Running Instructions:
 * Linux: Run run.sh
 * Windows: Install mingw and git, run run.sh using Git bash
 * Other: idk lmao go copy paste it into an online IDE or smth
 */

#include <inttypes.h>
#include <stdio.h>

#define packed __attribute__((packed))

typedef int32_t tc_t;
typedef int32_t pt_t;
typedef int32_t sg_t;
typedef int16_t mic_t;

typedef float imu_val_t;
typedef struct packed {
  imu_val_t x;
  imu_val_t y;
  imu_val_t z;
} imu_dof_t;

typedef struct packed {
  imu_dof_t acc;
  imu_dof_t gyro;
  imu_dof_t mag;
} imu_amg_t;

typedef imu_dof_t imu_pos_t;
typedef imu_dof_t imu_vel_t;

struct packed daughterboard_data {
  tc_t tcs[5];
  sg_t sgs[2];
  pt_t pts[1];
  imu_amg_t imu;
  imu_pos_t imu_pos;
  imu_vel_t imu_vel;
};

struct packed ecs_data { // engine control system
  pt_t pts[6];
};

struct packed vdaq_data {
  pt_t pts[5];
  tc_t tcs[3];
  sg_t sgs[4];
  imu_dof_t engine_acc;
};

struct packed main_data {
  struct daughterboard_data dbd;
  struct ecs_data ecs;
  struct vdaq_data vdaq;
};

uint32_t mainDataPeriod = 1000; // microseconds

// mic_t finCanMic;
// uint32_t finCanMicPeriod = 1000; // microseconds

size_t pageSize = 2048;   // bytes
size_t pageCount = 65536; // pages

int main() {
  // print sizes of main data and components
  printf("Main Data Size: %lu\n", sizeof(struct main_data));
  printf("Daughterboard Data Size: %lu\n", sizeof(struct daughterboard_data));
  printf("ECS Data Size: %lu\n", sizeof(struct ecs_data));
  printf("VDAQ Data Size: %lu\n", sizeof(struct vdaq_data));
  printf("\n");

  // print page size
  printf("Page Size: %lu\n", pageSize);

  size_t samplesPerPage = pageSize / sizeof(struct main_data);
  printf("Samples Per Page: %lu\n", samplesPerPage);
  float dataRate =
      (float)pageSize / (samplesPerPage * mainDataPeriod); // bytes/microsecond

  float dataRateKBps = dataRate * 1000.0; // kilobytes per second
  float dataRateKbps = dataRateKBps * 8;  // kilobits per second

  printf("Data Rate: %f KB/s\n", dataRateKBps);
  printf("Data Rate: %f Kbit/s\n", dataRateKbps);

  size_t totalSamples = samplesPerPage * pageCount;
  printf("Total Samples: %lu\n", totalSamples);
  printf("\n");

  // runtime
  uint64_t runtimeMicros = totalSamples * mainDataPeriod;
  float runtimeSeconds = runtimeMicros / 1000000.0f;
  float runtimeMinutes = runtimeSeconds / 60.0f;

  // print runtime
  printf("Runtime: %.2f seconds (%.2f minutes)\n", runtimeSeconds,
         runtimeMinutes);
}