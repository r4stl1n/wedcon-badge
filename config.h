#pragma once

#define CONFIG_FILE_NAME "/config.json"


struct ConfigData {
  struct {
    short channel;
    struct {
      unsigned long duration;
      unsigned long pause;
    } toast;
  } wifi;

  struct {
    unsigned long timeout;
  } hold;

  struct {
    struct {
      short low;
      short high;
    } luminance;

    struct {
      short hue;
      short saturation;
    } both;

    struct {
      short hue;
      short saturation;
    } bride;

    struct {
      short hue;
      short saturation;
    } groom;

    struct {
      short saturation;
      short luminance;
    } none;

    struct {
      short hue;
      short saturation;
      short luminance;
      unsigned long timeout;
    } toast;

    struct {
      unsigned long cycles;
    } intro;
  } led;

  struct {
    struct {
      int32_t low;
      int32_t high;
    } rssi;
    unsigned long timeout;
  } proximity;
};


void Config_Defaults();
bool Config_Load();
bool Config_Save();
void Config_Print();
const struct ConfigData& Config();
