//============================================================================
// Name        : ipadNav.cpp
// Author      : tom
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// This is a weird, lazy combination of C and C++. It works, so who cares.
/* Do not create default /logs folder.
	!Need another specified folder in the config file instead! */
#ifndef ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_NO_DEFAULT_LOG_FILE
#endif
#include "easylogging++.h"

#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <cmath>

#include <TransMatrix3d.h>

#include "trackingDevice.h"
#include "bluetoothDevice.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;


std::pair<bool,Vector3d> loadTipTransformation(std::string fileName) {
	pugi::xml_document settings;
	pugi::xml_parse_result result = settings.load_file(
			fileName.c_str());
	if (!result) {
		std::cout << "ERROR: Loading tip transformation from XML failed." << std::endl;
		std::cout << result.description() << std::endl;
		return std::make_pair<bool,Vector3d>(false,Vector3d(0,0,0));
	}

	pugi::xml_node tipTransNode = settings.child("tipTransformation");
	Vector3d tipTrans = Vector3d(atof(tipTransNode.child("x").child_value()),
			atof(tipTransNode.child("y").child_value()),
			atof(tipTransNode.child("z").child_value()));
	return std::make_pair<bool,Vector3d>(true,Vector3d(tipTrans.x(),tipTrans.y(),tipTrans.z()));;
}

std::pair<bool, Vector3d> setTrackedTool(trackingDevice* tracking_device, tracked_objects tracked_tool_id) {

	// We need to load the correct marker geometry & tip transformation
	// Marker geometry
	if (!tracking_device->changeLocators(static_cast<int>(tracked_tool_id))) {
		cout << "ERROR: Locator administrator could not change locators.\n" << endl;
		return std::make_pair<bool, Vector3d>(false, Vector3d(0,0,0));
	}

	// Tip transformation
	std::string tipTransformationFileName;
	if (tracked_tool_id == object_short_pointer) {
		tipTransformationFileName = "../Settings/tipTransformation_shortPointer.xml";
	} else if (tracked_tool_id == object_long_pointer) {
		tipTransformationFileName = "../Settings/tipTransformation_longPointer.xml";
	} else if (tracked_tool_id == object_robot_pointer) {
		tipTransformationFileName = "../Settings/tipTransformation_robotPointer.xml";
	} else if (tracked_tool_id == object_suction) {
		tipTransformationFileName = "../Settings/tipTransformation_suction.xml";
	} else {
		cout << "ERROR: tracked_tool_id is not known.\n" << endl;
		return std::make_pair<bool, Vector3d>(false, Vector3d(0,0,0));
	}

	std::pair<bool, Vector3d> tipTransformation = loadTipTransformation(tipTransformationFileName);
	if(!tipTransformation.first) {
		cout << "ERROR: Tip transformation could not be loaded.\n" << endl;
	}
	char msg[256];
	sprintf(msg, "Loaded tip transformation: %s", tipTransformationFileName.c_str());
	std::cout << msg << std::endl;

	return tipTransformation;
}

int main() {
	// Configure Loggers
	el::Configurations conf_defaultLogger("../logging/defaultLogger.conf");
	el::Configurations conf_encodedDataLogger("../logging/encodedDataLogger.conf");
	el::Configurations conf_rawDataLogger("../logging/rawDataLogger.conf");
	el::Loggers::reconfigureLogger("default", conf_defaultLogger); // Default logger
	el::Loggers::reconfigureLogger("encoded_data", conf_encodedDataLogger); // Logging encoded tracking data to file
	el::Loggers::reconfigureLogger("raw_data", conf_rawDataLogger); // Logging raw tracking data to file

	CLOG(INFO, "default") << "Start hardware communication program";

	tracked_objects last_tracked_object = object_short_pointer;


	trackingDevice* cambarB1 = new trackingDevice();
	double positionArray[12], quaternion[4];
	TransMatrix3d refPos, toolPos, toolInRef;

	Vector3d tipPos, tipTransformation;

	uint16_t simplifiedData[7] = { 0 };
	uint8_t bluetoothDataArray[17];

	char startByte = 0xFF;
	char endByte = 0xFE;

	// Values for simplification of positions
	float max_dist = 500;
	float offset_x = max_dist / 2, offset_y = max_dist / 2, offset_z = max_dist
			/ 2;
	float max_byte = pow(2.f, 16) - 1;
	// Values for simplification of orientation
	float quaternion_range = 2, quaternion_offset = 1;
	bool toolVisibilities[2] = { true, true };

	bluetoothDevice* btDev = new bluetoothDevice();
	//setBluetoothDevice(btDev);

	// Initialize camera & bluetooth connection
	bool couldInitializeCamera = cambarB1->initializeDevice();
	std::pair<bool, Vector3d> trackedTool = setTrackedTool(cambarB1, btDev->currentTrackedObject());
	if (couldInitializeCamera && trackedTool.first) {
		/* If initialisation of camera was correct continue
		   measuring locators and listen to changes of the tracked object until interrupt. */
		tipTransformation = trackedTool.second;


		while (true) {

			if (btDev->currentTrackedObject() !=  last_tracked_object) { // if remote changed tracked object, load & set geometry of new tracked object

				std::pair<bool, Vector3d> pair = setTrackedTool(cambarB1, btDev->currentTrackedObject());
				if (!pair.first) {
					std::cout << "ERROR: Tool could not be set." << std::endl;
					break;
				}
				tipTransformation = pair.second;
			}
			if (cambarB1->lockAndMeasureAllLocators()) {
				std::cout << "ERROR: Could not measure the locators." << std::endl;
				break;
			}
			LocatorResult ref = cambarB1->getLocatorResultAtIndex(
					trackingParams::referenceIndex);
			LocatorResult tool = cambarB1->getLocatorResultAtIndex(
					trackingParams::toolIndex);

			bluetoothDataArray[0] = startByte;
			bluetoothDataArray[1] = 0;

			if (ref.IsOk()) {
				ref.GetTranslation(positionArray[3], positionArray[7],
						positionArray[11]);
				ref.GetRotationAsMatrix(positionArray[0], positionArray[1],
						positionArray[2], positionArray[4], positionArray[5],
						positionArray[6], positionArray[8], positionArray[9],
						positionArray[10]);
				refPos = TransMatrix3d(positionArray);
				if (!toolVisibilities[trackingParams::referenceIndex]) {
					cout << "Reference visible" << endl;
				}
				toolVisibilities[trackingParams::referenceIndex] = true;
			} else {
				if (toolVisibilities[trackingParams::referenceIndex]) {
					cout << "Reference not visible" << endl;
				}
				toolVisibilities[trackingParams::referenceIndex] = false;
				bluetoothDataArray[1] += pow(2.f,
						(float) trackingParams::referenceIndex);
				bluetoothDataArray[2] = endByte;
				// Set LED to red
			}

			if (tool.IsOk()) {
				tool.GetTranslation(positionArray[3], positionArray[7],
						positionArray[11]);
				tool.GetRotationAsMatrix(positionArray[0], positionArray[1],
						positionArray[2], positionArray[4], positionArray[5],
						positionArray[6], positionArray[8], positionArray[9],
						positionArray[10]);
				toolPos = TransMatrix3d(positionArray);
				if (!toolVisibilities[trackingParams::toolIndex]) {
					cout << "Tool visible" << endl;
				}
				toolVisibilities[trackingParams::toolIndex] = true;

			} else {
				if (toolVisibilities[trackingParams::toolIndex]) {
					cout << "Tool not visible" << endl;
				}
				toolVisibilities[trackingParams::toolIndex] = false;
				// Set LED to red
				bluetoothDataArray[1] += pow(2.f,
						(float) trackingParams::toolIndex);
				bluetoothDataArray[2] = endByte;
			}

			if (tool.IsOk() && ref.IsOk()) {
				refPos.invert();
				toolInRef = refPos * toolPos;
				tipPos = toolInRef * tipTransformation;

				simplifiedData[0] = (uint16_t) (((tipPos.x() + offset_x)
						/ max_dist) * max_byte);
				simplifiedData[1] = (uint16_t) (((tipPos.y() + offset_y)
						/ max_dist) * max_byte);
				simplifiedData[2] = (uint16_t) (((tipPos.z() + offset_z)
						/ max_dist) * max_byte);

				toolInRef.getRotation().quaternion(quaternion);
				simplifiedData[3] = (uint16_t) (((quaternion[0]
						+ quaternion_offset) / quaternion_range) * max_byte);
				simplifiedData[4] = (uint16_t) (((quaternion[1]
						+ quaternion_offset) / quaternion_range) * max_byte);
				simplifiedData[5] = (uint16_t) (((quaternion[2]
						+ quaternion_offset) / quaternion_range) * max_byte);
				simplifiedData[6] = (uint16_t) (((quaternion[3]
						+ quaternion_offset) / quaternion_range) * max_byte);

				// Write to bluetooth
				memcpy(&bluetoothDataArray[2], &simplifiedData[0],
						7 * sizeof(uint16_t));
				memcpy(&bluetoothDataArray[16], &endByte, 1);

				//btDev->sendMessage(blablabal)
				/*if (state == state_connected) {

					CLOG(INFO, "encoded_data") << (int)bluetoothDataArray[0] << ";" << (int)bluetoothDataArray[1] << ";"
						<< (int)bluetoothDataArray[2] << ";" << (int)bluetoothDataArray[3] << ";"
						<< (int)bluetoothDataArray[4] << ";" << (int)bluetoothDataArray[5] << ";"
						<< (int)bluetoothDataArray[6] << ";" << (int)bluetoothDataArray[7] << ";"
						<< (int)bluetoothDataArray[8] << ";" << (int)bluetoothDataArray[9] << ";"
						<< (int)bluetoothDataArray[10] << ";" << (int)bluetoothDataArray[11] << ";"
						<< (int)bluetoothDataArray[12] << ";" << (int)bluetoothDataArray[13] << ";"
						<< (int)bluetoothDataArray[14] << ";" << (int)bluetoothDataArray[15] << ";"
						<< (int)bluetoothDataArray[16];
					CLOG(INFO, "raw_data") << tipPos.x() << ";" << tipPos.y() << ";"
						<< tipPos.z() << ";" << quaternion[0] << ";"
						<< quaternion[1] << ";" << quaternion[2] << ";"
						<< quaternion[3];
				}*/
			} /*else {
				if (state == state_connected) {

				}
			}*/
		}
	} else {
		// Blink LED
	}
	cambarB1->shutdownDevice();
	//bleRunning = false;
	return 0;
}
