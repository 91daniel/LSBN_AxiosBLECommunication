/*
 * baseTrackingDevice.h
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#ifndef INCLUDE_TRACKING_BASETRACKINGDEVICE_H_
#define INCLUDE_TRACKING_BASETRACKINGDEVICE_H_

class baseTrackingDevice {
public:
	//baseTrackingDevice();
	//virtual ~baseTrackingDevice();

	virtual bool initializeDevice() = 0;
	virtual void shutdownDevice() = 0;

	virtual int lockAndMeasureAllLocators() = 0;
	virtual TransMatrix3d getPose(unsigned int index) = 0;
	virtual bool isLocatorOK(unsigned int index) = 0;

	virtual unsigned int getNumberOfLoadedLocators() = 0;

	virtual bool changeLocators(int tracked_object) = 0;

private:

	//bool addLocators(int locator_id = 0);

	//std::vector<LocatorResult> m_locatorResults;

	//MetrologySystems * m_metSystems;
	//Metrology * m_met;
	//LocatorAdministrator * m_locatorAdmin;

};




#endif /* INCLUDE_TRACKING_BASETRACKINGDEVICE_H_ */
