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
#include "pugixml.hpp"

using namespace metro_lib;

namespace trackingParams {
enum markerIndices
{
	toolIndex = 0,
	referenceIndex = 1
};

}  // namespace trackingParams

class trackingDevice {
public:
	trackingDevice();
	~trackingDevice();

	bool initializeDevice();
	void shutdownDevice();

	int lock();
	int lockAndMeasureAllLocators();

	unsigned int getNumberOfLoadedLocators();
	LocatorResult getLocatorResultAtIndex(unsigned int index);

	bool changeLocators(int tracked_object);

private:

	bool addLocators(int locator_id = 0);

	std::vector<LocatorResult> m_locatorResults;

	MetrologySystems * m_metSystems;
	Metrology * m_met;
	LocatorAdministrator * m_locatorAdmin;

};

#endif /* TRACKINGDEVICE_H_ */

