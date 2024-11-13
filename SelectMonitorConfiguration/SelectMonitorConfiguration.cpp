// SelectMonitorConfiguration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "spdlog/spdlog.h"

#include <iostream>

#include <windows.h>
#include <winuser.h>
#include <physicalmonitorenumerationapi.h>

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT 1

BOOL enumDisplayCallback(HMONITOR hndl, HDC hdc, LPRECT rect, LPARAM monitorlist) {
  spdlog::info("Monitor found!");
  std::vector<HMONITOR>* monitors = (std::vector<HMONITOR>*)monitorlist;
  monitors->push_back(hndl);
  return TRUE;
}


struct MonitorInfo {
  std::string FriendlyDeviceName;
  std::string monitorDevicePath;
};


void try1() {
  std::vector<HMONITOR> monitorlist;
  auto result = EnumDisplayMonitors(NULL, NULL, enumDisplayCallback,
                                    LPARAM(&monitorlist));
  if (!result) {
    spdlog::error("EnumDisplayMonitors failed");
    return;
  }

  for (auto monitor : monitorlist) {
    DWORD n;
    BOOL result = GetNumberOfPhysicalMonitorsFromHMONITOR(monitor, &n);
    spdlog::info("Monitor has {} physical monitors", n);

    PHYSICAL_MONITOR physicalMonitors[1];
    auto retval = GetPhysicalMonitorsFromHMONITOR(monitor, 1, physicalMonitors);

    std::wstring monitorName(physicalMonitors[0].szPhysicalMonitorDescription);
    std::string monitorNameUtf8(monitorName.begin(), monitorName.end());
    spdlog::info("Monitor description: {}", monitorNameUtf8);
    DestroyPhysicalMonitors(1, physicalMonitors);
  }
}

void try2() {
  DISPLAY_DEVICE dd[1];
  DISPLAY_DEVICE dd2[1];
  dd[0].cb = sizeof(dd);
  dd2[0].cb = sizeof(dd2);
  int deviceIndex = 0;

  while (EnumDisplayDevices(NULL, deviceIndex, dd, 0)) {
    std::wstring deviceName(dd[0].DeviceName);
    int monitorIndex = 0;
    while (EnumDisplayDevices(dd[0].DeviceName, monitorIndex, dd2, 0)) {
      std::wstring monitorName(dd2[0].DeviceName);
      std::string monitorNameUtf8(monitorName.begin(), monitorName.end());
      std::wstring monitorString(dd2[0].DeviceString);
      std::string monitorStringUtf8(monitorString.begin(), monitorString.end());
      std::wstring monitorDeviceId(dd2[0].DeviceID);
      std::string monitorDeviceIdUtf8(monitorDeviceId.begin(), monitorDeviceId.end());
      std::wstring monitorDeviceKey(dd2[0].DeviceKey);
      std::string monitorDeviceKeyUtf8(monitorDeviceKey.begin(), monitorDeviceKey.end());
      // std::cout << dd2[0].DeviceName << ", " << dd2[0].DeviceString << "\n";

      spdlog::info("Name: {0}, string: {1}, ID: {2}, Key:{3}", monitorNameUtf8,
                   monitorStringUtf8, monitorDeviceIdUtf8, monitorDeviceKeyUtf8);
      ++monitorIndex;
    }
    ++deviceIndex;
  }
}

void QueryDisplay() {
  std::vector<DISPLAYCONFIG_PATH_INFO> paths;
  std::vector<DISPLAYCONFIG_MODE_INFO> modes;
  UINT32 flags = QDC_ONLY_ACTIVE_PATHS;
  LONG isError = ERROR_INSUFFICIENT_BUFFER;

  UINT32 pathCount, modeCount;
  isError = GetDisplayConfigBufferSizes(flags, &pathCount, &modeCount);

  if (isError) {
    return;
  }

  // Allocate the path and mode arrays
  paths.resize(pathCount);
  modes.resize(modeCount);

  // Get all active paths and their modes
  isError = QueryDisplayConfig(flags, &pathCount, paths.data(), &modeCount,
                               modes.data(), nullptr);

  // The function may have returned fewer paths/modes than estimated
  paths.resize(pathCount);
  modes.resize(modeCount);

  if (isError) {
    return;
  }

  // For each active path
  int len = paths.size();
  for (int i = 0; i < len; i++) {
    // Find the target (monitor) friendly name
    DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {};
    targetName.header.adapterId = paths[i].targetInfo.adapterId;
    targetName.header.id = paths[i].targetInfo.id;
    targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
    targetName.header.size = sizeof(targetName);
    isError = DisplayConfigGetDeviceInfo(&targetName.header);

    if (isError) {
      return;
    }
    std::string mon_name = "Unknown";
    if (targetName.flags.friendlyNameFromEdid) {
      std::wstring wbuf(targetName.monitorFriendlyDeviceName);
      mon_name = std::string(wbuf.begin(), wbuf.end());
      // mon_name = QString::fromStdWString(targetName.monitorFriendlyDeviceName);
    }

    spdlog::info("Monitor: {}", mon_name);
  }
}

int main()
{

  spdlog::info("moncfg started");
  try1();
  try2();
  QueryDisplay();


  
  return 0;
}
