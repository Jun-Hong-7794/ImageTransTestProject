#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>	
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "CUDP.h"
#include "CSocket.h"

#define UNIT_BUFFER_SIZE 60000
#define TOTAL_BUFFER_SIZE 810000

bool checkpackage(unsigned char* _input, unsigned char* _output, int _loop_num, int& _total_buffer_size, bool& _is_end_package);

int main() {

	CUDP my_udp;
	if(!my_udp.UDP_init(ONLY_MY, "192.168.1.11", 1230, "OpencvImageTest", "127.0.0.1", 1232))
		return 0;

	cv::Mat tmp_trans_img;

	unsigned char tmp_buffer[UNIT_BUFFER_SIZE];
	unsigned char total_buffer[TOTAL_BUFFER_SIZE];
	
	memset(tmp_buffer, 0, UNIT_BUFFER_SIZE);
	memset(total_buffer, 0, TOTAL_BUFFER_SIZE);
	
	int loop_counter = 0;
	int total_buffer_size = 0;
	bool package_check = false;
	bool end_package_flag = false;
	
	while (true) {
		
		while(true) {
			if (my_udp.Recv(tmp_buffer, UNIT_BUFFER_SIZE) > 0) {
				
				if (!checkpackage(tmp_buffer, total_buffer, loop_counter, total_buffer_size, end_package_flag)) {
					package_check = false;
					break;
				}
				else {
					package_check = true;
					
					if (end_package_flag)
						break;
				}
				loop_counter++;
			}
		}
		if (!package_check) {
			//Go back to while loop
			memset(tmp_buffer, 0, UNIT_BUFFER_SIZE);
			memset(total_buffer, 0, TOTAL_BUFFER_SIZE);
			total_buffer_size = 0;
			loop_counter = 0;
			end_package_flag = false;
			printf("Fail to Package");
			continue;
		}
		
		//unsigned char* tmp_char_buffer = new unsigned char[total_buffer_size];
		std::vector<uchar> tmp_uchar(total_buffer, total_buffer + total_buffer_size);
		
		tmp_trans_img = cv::imdecode(tmp_uchar, 1);
		cv::imshow("ShowImage", tmp_trans_img);
		memset(tmp_buffer, 0, UNIT_BUFFER_SIZE);
		memset(total_buffer, 0, TOTAL_BUFFER_SIZE);
		total_buffer_size = 0;
		loop_counter = 0;
		end_package_flag = false;
		cv::waitKey(1);
	}
	my_udp.CleanSocket();
	getchar();

	return 0;
}
// Header(3) + loopNum(4) + dataSize(4) + **Data(7986)** + Tail(3) = 8000
bool checkpackage(unsigned char* _input, unsigned char* _output, int _loop_num, int& _total_buffer_size, bool& _is_end_package) {

	//_input => _output
	if(!((_input[0] == 'M') && (_input[1] == 'O') && (_input[2] == 'R')))//Check Header
		return false;
	
	if (!((_input[UNIT_BUFFER_SIZE - 2] == 'A') && (_input[UNIT_BUFFER_SIZE - 1] == 'I'))) {//Check Tail
		if (_input[UNIT_BUFFER_SIZE - 2] != 'E')
			return false;
		else
			_is_end_package = true;
	}

	int block_num = 0;
	memcpy(&block_num, &_input[3], sizeof(int));

	if (block_num != _loop_num)//Unsinc
		return false;

	int block_data_size = 0;
	memcpy(&block_data_size, &_input[7], sizeof(int));

	if ((_total_buffer_size + block_data_size) > TOTAL_BUFFER_SIZE) {
		printf("Size over: total buffer");
		return false;
	}

	static int prev_block_size = 0;
	memcpy(&_output[_total_buffer_size], &_input[11], block_data_size);
	
	_total_buffer_size += block_data_size;
	prev_block_size = block_data_size;

	return true;
}

