//
// Created by gilledevr12 on 2/5/18.
//

#include "dwm_api/deca_device_api.h"
#include <stdio.h>

int main(){
	uint32 id = 0;
	//uint8 *pointer;
	id = dwt_readdevid();
	//for (int i = 3; i >= 0; i--){
		printf("%lx \n", id); 
//	}
}
