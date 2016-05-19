/*
 * test.cpp
 *
 *  Created on: 2016年5月18日
 *      Author: tianshan
 */

#include <iostream>
#include "cdnbalance.h"

using namespace std;

int main() {

	std::string avatar = "ap/a/0a/65/1ec8dd2ed3f1c9db041d64633e93_174_174.cg.png";

	cout << CDNBalance::getInstance()->_img(avatar);

//	CDNBalance::getInstance()->get_cdn_url();
	return 0;
}


