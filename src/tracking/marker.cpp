/*
 * marker.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: tom
 */

#include "marker.h"

marker::marker(char markerID, int trackingResult) {
	m_markerID = markerID;
	m_trackingResult = trackingResult;
	m_trackedPosition = TransMatrix3d::getIdentity();
}

marker::marker(char markerID, int trackingResult, TransMatrix3d trackedPosition) {
	m_markerID = markerID;
	m_trackingResult = trackingResult;
	m_trackedPosition = trackedPosition;
}

marker::~marker() {

}



