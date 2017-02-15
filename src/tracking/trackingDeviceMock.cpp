/*
 * trackingDeviceMock.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#include "trackingDeviceMock.h"
#include <stdio.h>

trackingDeviceMock::trackingDeviceMock() {}

trackingDeviceMock::~trackingDeviceMock() {}

bool trackingDeviceMock::initializeDevice() {
	return true;
}

void trackingDeviceMock::shutdownDevice() {}

bool trackingDeviceMock::changeLocators(int tracked_object) {
	return true;
}

int trackingDeviceMock::lockAndMeasureAllLocators() {
	return 0;
}

TransMatrix3d trackingDeviceMock::getPose(unsigned int index) {
	return TransMatrix3d().getIdentity();
}

bool trackingDeviceMock::isLocatorOK(unsigned int index) {
	return true;
}

unsigned int trackingDeviceMock::getNumberOfLoadedLocators() {
	return 2;
}





