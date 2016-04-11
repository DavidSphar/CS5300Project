/*
 * LabelManager.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tamnguyen
 */

#ifndef LABELMANAGER_H_
#define LABELMANAGER_H_

#include <vector>
#include <memory>

class LabelManager {
public:
	std::vector<int> ifLabelStack;
	std::vector<int> controlLabelStack;
	int controlLabels = 0;
};


#endif /* LABELMANAGER_H_ */
