#pragma once
#include <geometry_msgs/Vector3Stamped.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/Char.h>

#include <teraranger_array/teraranger_core.h>
#include <teraranger_array/TerarangerHubEvoConfig.h>
#include <multiple_asynchronous_timers/AsyncTimerArray.h>

#define BUFFER_SIZE 38 //max frame length
#define RANGES_FRAME_LENGTH 20
#define RANGE_CRC_POS 19
#define IMU_QUAT_FRAME_LENGTH 12
#define IMU_EULER_FRAME_LENGTH 10
#define IMU_QUATLIN_FRAME_LENGTH 18

#define SERIAL_SPEED 115200
#define SERIAL_TIMEOUT_MS 1000

#define OUT_OF_RANGE_VALUE -1
#define TOO_CLOSE_VALUE 0
#define INVALID_MEASURE_VALUE 1
#define VALUE_TO_METER_FACTOR 0.001

#define ACK_LENGTH 4
#define ACK_HEADER 0x30
#define NACK_VALUE 0xFF
#define ACK_VALUE 0x00

#define NB_SENSORS 8

namespace teraranger_array
{
// Protocol commands
static const char ENABLE_CMD[5] = {(char)0x00, (char)0x52, (char)0x02, (char)0x01, (char)0xDF};
static const char DISABLE_CMD[5] = {(char)0x00, (char)0x52, (char)0x02, (char)0x00, (char)0xD8};

static const char TEXT_MODE[4] = {(char)0x00, (char)0x11, (char)0x01, (char)0x45};
static const char BINARY_MODE[4] = {(char)0x00, (char)0x11, (char)0x02, (char)0x4C};

static const char CROSSTALK_MODE[4] = {(char)0x00, (char)0x31,(char)0x01,(char)0xEB}; // All sensors continously
static const char NONCROSSTALK_MODE[4] = {(char)0x00, (char)0x31, (char)0x02, (char)0xE2}; // All sensors sequentially

static const char IMU_OFF[4] = {(char)0x00, (char)0x41, (char)0x01, (char)0x49};
static const char IMU_QUAT[4] = {(char)0x00, (char)0x41, (char)0x02, (char)0x40};
static const char IMU_EULER[4] = {(char)0x00, (char)0x41, (char)0x03, (char)0x47};
static const char IMU_QUATLIN[4] = {(char)0x00, (char)0x41, (char)0x04, (char)0x52};

static const char RATE_ASAP[5] = {(char)0x00, (char)0x52, (char)0x03,(char)0x01, (char)0xCA};
static const char RATE_50[5] = {(char)0x00, (char)0x52, (char)0x03, (char)0x02,(char)0xC3};
static const char RATE_100[5] = {(char)0x00, (char)0x52, (char)0x03, (char)0x03,(char)0xC4};
static const char RATE_250[5] = {(char)0x00, (char)0x52, (char)0x03, (char)0x04,(char)0xD1};

enum imu_mode{
  off,
  quat,
  euler,
  quatlin
};

class TerarangerHubEvo
{
public:
  TerarangerHubEvo();
  virtual ~TerarangerHubEvo();

  void serialDataCallback(uint8_t data);

  void dynParamCallback(const teraranger_evo_cfg::TerarangerHubEvoConfig &config, uint32_t level);

  bool loadParameters();
  void setMode(const char *c, int length);
  void spin();

  ros::NodeHandle nh_;
  ros::Publisher range_publisher_;
  ros::Publisher imu_publisher_;
  ros::Publisher euler_publisher_;

  dynamic_reconfigure::Server<teraranger_evo_cfg::TerarangerHubEvoConfig> dyn_param_server_;
  dynamic_reconfigure::Server<teraranger_evo_cfg::TerarangerHubEvoConfig>::CallbackType dyn_param_server_callback_function_;

  boost::function<void(uint8_t)> serial_data_callback_function_;

  std::string portname_;
  std::string ns_;

private:
  float field_of_view ;
  float max_range;
  float min_range;
  int number_of_sensor;
  std::string frame_id;
  serial::Serial serial_port_;
  imu_mode imu_status;
  int current_imu_frame_length;

  teraranger_array::RangeArray range_array_msg;
  sensor_msgs::Imu imu_msg;
  geometry_msgs::Vector3Stamped euler_msg;

  void processRangeFrame(uint8_t* input_buffer, int seq_ctr);
  void processImuFrame(uint8_t* input_buffer, int seq_ctr);
  bool processAck(uint8_t* ack_buffer, const uint8_t* cmd);

  AsyncTimerArray* sensor_timers = nullptr;
  std::vector<bool> required_sensors_mask_;
  int nan_timeout_;
  void validate_sensor(int sensor_id);
  void invalidate_sensor(int sensor_id);
  void check_timers();
};

} // namespace teraranger_array
