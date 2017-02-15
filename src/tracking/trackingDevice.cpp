/*
 * trackingDevice.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: tom
 */

#include "trackingDevice.h"
#include <stdio.h>

trackingDevice::trackingDevice() :
		m_metSystems( NULL), m_met( NULL), m_locatorAdmin( NULL) {
	m_metSystems = new MetrologySystems();
	m_locatorAdmin = new LocatorAdministrator();
	m_met = NULL;

}
trackingDevice::~trackingDevice() {
	shutdownDevice();
}

bool trackingDevice::initializeDevice() {
	try {
		MeasurementSystemInfoList msil = m_metSystems->FindMeasurementSystems();
		char msg[256];
		sprintf(msg, "Detected %d system(s)", msil.Size());
		std::cout << msg << std::endl;

		if (msil.Size() > 0) {
			MeasurementSystemInfo msi = msil.GetElement(0);  // Assuming we want the first system
			sprintf(msg, "System Name: %s", msi.GetSystemName().c_str());
			std::cout << msg << std::endl;
			sprintf(msg, "Serial Number: %s", msi.GetSerialNumber().c_str());
			std::cout << msg << std::endl;
			sprintf(msg, "Configuration: %d", msi.GetConfiguration());
			std::cout << msg << std::endl;
			sprintf(msg, "Handle: %d", msi.GetHandle());
			std::cout << msg << std::endl;

			m_met = new Metrology(m_metSystems->InitSystem(msi));
			m_met->SystemCheck();


			if (addLocators()) {
				return true;
			} else {
				std::cout << "Tracking initialization failed adding locators" << std::endl;
				return false;
			}

		} else {
			return false;
		}
	} catch (MetroError &e) {
		char msg[256];
		sprintf(msg, "Error in tracking system init: %s", e.GetMergedInfoString().c_str());
		std::cout << msg << std::endl;
		return false;
	}

}

void trackingDevice::shutdownDevice() {
	m_locatorAdmin->DeleteAllLocators();
	delete m_metSystems;
	m_metSystems = NULL;
	delete m_locatorAdmin;
	m_locatorAdmin = NULL;
	if (m_met != NULL) {
		m_met->Delete();
		delete m_met;
	}
}

int trackingDevice::lock() {
	try {
		m_met->Lock();
		return 0;
	} catch (MetroError &e) {
		char msg[256];
		sprintf(msg, "Error during tracking: %d", e.GetCode(0));
		std::cout << msg << std::endl;
		return e.GetCode(0);
	}
}

bool trackingDevice::addLocators(int locator_id) {
	pugi::xml_document settings;
	pugi::xml_parse_result result = settings.load_file("../Settings/locators.xml");
	if (!result) {
		std::cout << "Error loading locators paths from XML" << std::endl;
		std::cout << result.description() << std::endl;
		return false;
	}

	pugi::xml_node locatorPaths;
	if (locator_id == 0) {
		locatorPaths = settings.child("shortPointerLocatorPaths");
	} else if (locator_id == 1) {
		locatorPaths = settings.child("longPointerLocatorPaths");
	} else if (locator_id == 2) {
		locatorPaths = settings.child("robotPointerLocatorPaths");
	} else if (locator_id == 3) {
		locatorPaths = settings.child("suctionLocatorPaths");
	} else {
		std::cout << "ERROR: Locator id is unknown." << std::endl;
	}

	for (pugi::xml_node locatorPath = locatorPaths.child("locatorPath"); locatorPath; locatorPath = locatorPath.next_sibling("locatorPath")) {
		char msg[256];
		sprintf(msg, "Adding locator: %s", locatorPath.child_value());
		std::cout << msg << std::endl;
		try {
			m_locatorAdmin->LoadLocatorsFromFile(locatorPath.child_value());
		} catch (MetroError &e) {
			std::cout << "Error adding locator" << std::endl;
			return false;
		}
	}

	return true;

}

bool trackingDevice::changeLocators(int tracked_object) {
	m_locatorAdmin->DeleteAllLocators();
	if (addLocators(tracked_object)) {
		return true;
	} else {
		std::cout << "Changing locators failed." << std::endl;
		return false;
	}
}

int trackingDevice::lockAndMeasureAllLocators() {
	try {
		m_met->Lock();
		for (unsigned int i = 0; i < m_locatorAdmin->NumberOfLoadedLocators(); i++) {
			if (m_locatorResults.size() <= i) {
				m_locatorResults.push_back(m_met->MeasureLocator(m_locatorAdmin->GetLocatorByIndex(i).GetId().c_str()));
			} else {
				m_locatorResults[i] = m_met->MeasureLocator(m_locatorAdmin->GetLocatorByIndex(i).GetId().c_str());
			}
		}
		return 0;
	} catch (MetroError &e) {
		char msg[256];
		sprintf(msg, "Error during tracking: %d", e.GetCode(0));
		std::cout << msg << std::endl;
		return e.GetCode(0);
	}
}

LocatorResult trackingDevice::getLocatorResultAtIndex(unsigned int index) {
	if (index < m_locatorResults.size()) {
		return m_locatorResults[index];
	} else {
		std::cout << "The requested tracker index is too high, returning the last locator result because I have to return something now don't I" << std::endl;
		return m_locatorResults[m_locatorResults.size() - 1];
	}
}

unsigned int trackingDevice::getNumberOfLoadedLocators() {
	return m_locatorAdmin->NumberOfLoadedLocators();
}
