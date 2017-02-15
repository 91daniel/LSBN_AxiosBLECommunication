/*
 * trackingDeviceMock.h
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#ifndef INCLUDE_TRACKING_TRACKINGDEVICEMOCK_H_
#define INCLUDE_TRACKING_TRACKINGDEVICEMOCK_H_

#include <TransMatrix3d.h>
#include "baseTrackingDevice.h"

class trackingDeviceMock: public baseTrackingDevice {
public:
	trackingDeviceMock();
	~trackingDeviceMock();

	bool initializeDevice();
	void shutdownDevice();

	int lockAndMeasureAllLocators();
	TransMatrix3d getPose(unsigned int index);
	bool isLocatorOK(unsigned int index);

	bool changeLocators(int tracked_object);

	unsigned int getNumberOfLoadedLocators();


};





#endif /* INCLUDE_TRACKING_TRACKINGDEVICEMOCK_H_ */
