/*
 * trackingDevice.h
 *
 *  Created on: Sep 2, 2014
 *      Author: tom
 */

#ifndef TRACKINGDEVICE_H_
#define TRACKINGDEVICE_H_

#include "MetrologySystems.h"
#include "MetroError.h"
#include <iostream>
#include <vector>
#include <TransMatrix3d.h>

#include "pugixml.hpp"
#include "baseTrackingDevice.h"

using namespace metro_lib;

namespace trackingParams {
enum markerIndices
{
	toolIndex = 0,
	referenceIndex = 1
};

}  // namespace trackingParams

class trackingDevice: public baseTrackingDevice  {
public:
	trackingDevice();
	~trackingDevice();

	bool initializeDevice();
	void shutdownDevice();

	int lock();
	int lockAndMeasureAllLocators();
	Vector3d getTranslation(unsigned int index);
	Matrix3x3 getRotationMatrix(unsigned int index);
	TransMatrix3d getPose(unsigned int index);
	bool isLocatorOK(unsigned int index);

	unsigned int getNumberOfLoadedLocators();

	bool changeLocators(int tracked_object);

private:
	LocatorResult getLocatorResultAtIndex(unsigned int index);

	bool addLocators(int locator_id = 0);

	std::vector<LocatorResult> m_locatorResults;

	MetrologySystems * m_metSystems;
	Metrology * m_met;
	LocatorAdministrator * m_locatorAdmin;

};

#endif /* TRACKINGDEVICE_H_ */

