defaulti: 2-initial

initial:
	gcc -std=c11 decadriver/dwm_api/* init_ss.c -l mosquitto -o init

response:
	gcc -std=c11 decadriver/dwm_api/* ss_resp.c -l mosquitto -o resp

2-initial:
	gcc -std=c11 decadriver/dwm_api/* 2-init.c -l mosquitto -o 2-init

2-response:
	gcc -std=c11 decadriver/dwm_api/* 2-resp.c -l mosquitto -o 2-resp
