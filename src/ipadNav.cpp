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
#include "cmd_def.h"
#include "uart.h"
INITIALIZE_EASYLOGGINGPP

using namespace std;

#define TRACKING_DATA_HANDLE 8
#define OBJECT_ID_HANDLE 13

extern "C" {

typedef enum {
	state_disconnected,
	state_connected,
	state_finish
} states;
states state = state_disconnected;

typedef enum {
	object_short_pointer,
	object_long_pointer,
	object_robot_pointer,
	object_suction
} tracked_objects;
tracked_objects tracked_object = object_short_pointer;

void change_state(states new_state)
{
#ifdef _DEBUG
	printf("DEBUG: State changed: %i --> %i\n", static_cast<int>(state), static_cast<int>(new_state));
#endif
	state = new_state;
}

void change_tracked_object(tracked_objects new_tracked_object)
{
#ifdef _DEBUG
	printf("New tracked object %i", static_cast<int>(new_tracked_object));
#endif
	tracked_object = new_tracked_object;
}

void print_raw_packet(struct ble_header *hdr, unsigned char *data)
{
	printf("Incoming packet: ");
	int i;
	for (i = 0; i < sizeof(*hdr); i++) {
		printf("%02x ", ((unsigned char *)hdr)[i]);
	}
	for (i = 0; i < hdr->lolen; i++) {
		printf("%02x ", data[i]);
	}
	printf("\n");
}


/**
 * Send BGAPI packet using UART interface
 *
 * @param len1 Length of fixed portion of packet (always at least 4)
 * @param data1 Fixed-length portion of BGAPI packet (should always be <len1> bytes long)
 * @param len2 Length of variable portion of packet data payload (trailing uint8array or uint16array)
 * @param data2 Variable-length portion of data payload (should always be <len2> bytes long)
 */
void send_api_packet(uint8 len1, uint8* data1, uint16 len2, uint8* data2) {
#ifdef _DEBUG
	// display outgoing BGAPI packet
	print_raw_packet((struct ble_header *)data1, data2);
#endif

	// transmit complete packet via UART
	if (uart_tx(len1, data1) || uart_tx(len2, data2)) {
		// uart_tx returns non-zero on failure
		printf("ERROR: Writing to serial port failed\n");
	}
}

/**
 * Receive BGAPI packet using UART interface
 *
 * @param timeout_ms Milliseconds to wait before timing out on the UART RX operation
 */
int read_api_packet(int timeout_ms) {
	unsigned char data[256]; // enough for BLE
	struct ble_header hdr;
	int r;

	r = uart_rx(sizeof(hdr), (unsigned char *) &hdr, timeout_ms);
	if (!r) {
		return -1; // timeout
	} else if (r < 0) {
		printf("ERROR: Reading header failed. Error code:%d\n", r);
		return 1;
	}

	if (hdr.lolen) {
		r = uart_rx(hdr.lolen, data, timeout_ms);
		if (r <= 0) {
			printf("ERROR: Reading data failed. Error code:%d\n", r);
			return 1;
		}
	}

	// use BGLib function to create correct ble_msg structure based on the header
	// (header contains command class/ID info, used to identify the right structure to apply)
	const struct ble_msg *msg = ble_get_msg_hdr(hdr);

#ifdef DEBUG
	// display incoming BGAPI packet
	print_raw_packet(&hdr, data);
#endif

	if (!msg) {
		printf("ERROR: Unknown message received\n");
	}

	// call the appropriate handler function with any payload data
	// (this is what triggers the ble_evt_* and ble_rsp_* functions)
	msg->handler(data);

	return 0;
}

void advertiseBle() {
	ble_cmd_gap_set_adv_parameters(32 /* adv_interval_min */, 48 /* adv_interval_max */, 7 /* adv_channels */);
	ble_cmd_gap_set_mode(gap_general_discoverable, gap_undirected_connectable);

}

void ble_rsp_system_hello(const void* nul)
{
	printf("Device responded to hello message and is functional.\n");
}

void ble_evt_connection_status(const struct ble_msg_connection_status_evt_t *msg)
{
	// New connection
	if (msg->flags & connection_connected) {
		change_state(state_connected);
		printf("Connected.\n");
	}
	else
	{
		printf("Not connected.\n");
	}
}
//void ble_evt_connection_status(
//		const struct ble_msg_connection_status_evt_t *msg) {
//	printf("Connected\n");
	//ble_cmd_connection_update(0,16,17,0,0);
//	connectionStatus = CONNECTED;
//}

void ble_evt_connection_disconnected(
	const struct ble_msg_connection_disconnected_evt_t *msg) {
	change_state(state_disconnected);
	printf("Connection terminated, continue advertising.\n");
	advertiseBle();
}
//void ble_evt_connection_disconnected(
//		const struct ble_msg_connection_disconnected_evt_t *msg) {
//	printf("Disconnected\n");
//	connectionStatus = DISCONNECTED;
//	advertiseBle();
//}

void ble_evt_attributes_value(const struct ble_msg_attributes_value_evt_t * msg)
{
	printf("Local attribute value was written by a remote device.\n");
	if (msg->handle == OBJECT_ID_HANDLE)
	{
		uint16 offset = msg->offset;
		for (uint8 i = 0; i < msg->value.len; i++) // i should only take the value 0!
		{
			change_tracked_object(static_cast<tracked_objects>(msg->value.data[i]));
		}
	}
}
} /* end C code */

bool bleRunning = false;

void *run_ble(void *ptr) {

	bleRunning = true;
	while (bleRunning) {
		read_api_packet(100 /* timeout */);
	}
	return NULL;
}

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

	// Open serial port for writing
	if (uart_open("/dev/ttyACM0")) {
		cout << "ERROR: Unable to open serial port\n" << endl;
		return 1;
	}

	bglib_output = send_api_packet;

	ble_cmd_connection_disconnect(0);
	advertiseBle();

	/* this variable is our reference to the second thread */
	pthread_t ble_thread;
	if (pthread_create(&ble_thread, NULL, run_ble, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

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

	// Initialize camera & bluetooth connection
	bool couldInitializeCamera = cambarB1->initializeDevice();
	std::pair<bool, Vector3d> trackedTool = setTrackedTool(cambarB1, tracked_object);
	if (couldInitializeCamera && trackedTool.first) {
		/* If initialisation of camera was correct continue
		   measuring locators and listen to changes of the tracked object until interrupt. */
		tipTransformation = trackedTool.second;

		while (state != state_finish) {

			if (tracked_object !=  last_tracked_object) { // if remote changed tracked object, load & set geometry of new tracked object

				std::pair<bool, Vector3d> pair = setTrackedTool(cambarB1, tracked_object);
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

				if (state == state_connected) {
					ble_cmd_attributes_send(0, TRACKING_DATA_HANDLE, 17,
							&bluetoothDataArray);

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
				}
			} else {
				if (state == state_connected) {
					ble_cmd_attributes_send(0, TRACKING_DATA_HANDLE, 3,
							&bluetoothDataArray);
				}
			}
		}
	} else {
		// Blink LED
	}
	cambarB1->shutdownDevice();
	bleRunning = false;
	return 0;
}
