/*
 * marker.h
 *
 *  Created on: Oct 10, 2014
 *      Author: tom
 */

#ifndef MARKER_H_
#define MARKER_H_

#include "TransMatrix3d.h"

class marker
{
public:
	marker(char markerID, int trackingResult, TransMatrix3d position);
	marker(char markerID, int trackingResult);
	~marker();

	TransMatrix3d getTrackedPosition(){return m_trackedPosition;};
	int getTrackingResult(){return m_trackingResult;};
	char getMarkerID(){return m_markerID;};

	void setTrackedPosition(TransMatrix3d trackedPosition){m_trackedPosition = trackedPosition;};
	void setTrackingResult(int trackingResult){m_trackingResult = trackingResult;};
	void setMarkerID(char markerID){m_markerID = markerID;};

private:
	TransMatrix3d 	m_trackedPosition;
	int 			m_trackingResult;
	char			m_markerID;
};



#endif /* MARKER_H_ */
