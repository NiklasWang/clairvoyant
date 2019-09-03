#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "CameraParameters.h"
#include "Logs.h"
#include "Common.h"

namespace pandora {


CameraParameters::CameraParameters()
{
}

CameraParameters::~CameraParameters()
{
}


void CameraParameters::set(int key, int value)
{
	if(key <= KEY_INVALID && key >= 0) {
		KEY[key] = value;
	}
}


int CameraParameters::get(int key) const
{
	if(key <= KEY_INVALID && key >= 0) {
		return KEY[key];
	}
    return -1;
}

};
