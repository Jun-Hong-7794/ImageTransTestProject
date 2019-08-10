#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>	
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "CUDP.h"
#include "CSocket.h"

#define UNIT_BUFFER_SIZE 65000
#define TOTAL_BUFFER_SIZE 810000
#define NETWORK_DISCONNECTION 0x02

//#define USINGTCP

bool checkUDPpackage(unsigned char* _input, unsigned char* _output, int _loop_num, int& _total_buffer_size, bool& _is_end_package);
int parseHeader(CClient* _client_socket, int& _image_data_size);

int main() {
	
#ifdef USINGTCP
	CClient my_client;
	
	if(my_client.Client_init(1239, "192.168.0.11") == SOCKET_SUCCESS)
		printf("Client Init");
	else {
		printf("Fail to Client Init");
		return 0;
	}

#else
	CUDP my_udp;
	int dst_port = 2230;
	int host_port = 1232;
	if (!my_udp.UDP_init(ONLY_MY, "127.0.0.1", dst_port, "OpencvImageTest", "127.0.0.1", host_port))
		return 0;
#endif // USINGTCP

	cv::Mat tmp_trans_img;
	unsigned char tmp_buffer[UNIT_BUFFER_SIZE];
	
#ifdef USINGTCP
	unsigned char* total_buffer;
#else
	unsigned char total_buffer[TOTAL_BUFFER_SIZE];
#endif
	memset(tmp_buffer, 0, UNIT_BUFFER_SIZE);

#ifdef USINGTCP
#else
	memset(total_buffer, 0, TOTAL_BUFFER_SIZE);
#endif
	
	
	int loop_counter = 0;
	int total_buffer_size = 0;
	bool package_check = false;
	bool end_package_flag = false;
	
	while (true) {
		
		while(true) {
#ifdef USINGTCP
			if (parseHeader(&my_client, total_buffer_size) != NETWORK_DISCONNECTION) {
			
				if (total_buffer_size <= 0)
					continue;

				int received_data_size = 0;
				total_buffer = new unsigned char[total_buffer_size];
				
				if (my_client.Receive_Data((char *)total_buffer, sizeof(BYTE) * total_buffer_size, received_data_size) == SOCKET_RV_FAIL) {
					printf("Receive Error");
					
					my_client.CloseClient();
					
					if (my_client.Client_init(1233, "127.0.0.1") == SOCKET_SUCCESS)
						printf("Client Init");

					continue;
				}
				break;
			}
			else {
				continue;
			}
#else
			if (my_udp.Recv(tmp_buffer, UNIT_BUFFER_SIZE) > 0) {

				if (!checkUDPpackage(tmp_buffer, total_buffer, loop_counter, total_buffer_size, end_package_flag)) {
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
#endif // USINGTCP
		}

#ifdef USINGTCP

#else
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
#endif // USINGTCP
		//unsigned char* tmp_char_buffer = new unsigned char[total_buffer_size];
		std::vector<uchar> tmp_uchar(total_buffer, total_buffer + total_buffer_size);
		tmp_trans_img = cv::imdecode(tmp_uchar, 1);
		//cv::imwrite("JunTest.jpg", tmp_trans_img);
		cv::imshow("ShowImage", tmp_trans_img);
		cv::waitKey(1);
		
#ifdef USINGTCP
		delete[] total_buffer;
#else
		memset(tmp_buffer, 0, UNIT_BUFFER_SIZE);
		memset(total_buffer, 0, TOTAL_BUFFER_SIZE);
		total_buffer_size = 0;
		loop_counter = 0;
		end_package_flag = false;
#endif // USINGTCP
	}
	

#ifdef USINGTCP
	//Clean on destruction
#else
	my_udp.CleanSocket();
#endif // USINGTCP

	getchar();

	return 0;
}

// UDP
// Header(3) + block_num(4) + dataSize(4) + **Data(7986)** + Tail(3) = 8000
bool checkUDPpackage(unsigned char* _input, unsigned char* _output, int _loop_num, int& _total_buffer_size, bool& _is_end_package) {

	if(!((_input[0] == 'M') && (_input[1] == 'O') && (_input[2] == 'R')))//Check Header
		return false;
	
	if (!((_input[UNIT_BUFFER_SIZE - 2] == 'A') && (_input[UNIT_BUFFER_SIZE - 1] == 'I'))) {//Check Tail. AI(Normal Package Type) or AE(When the package is last of Image data package)
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

// TCP/IP
int parseHeader(CClient* _client_socket, int& _image_data_size) {

	int received_data_size = 0;
	BYTE header_data[5];
	BYTE magic_word[5] = { 'M', 'O', 'R', 'A', 'I' };
	
	memset(header_data, 0, sizeof(BYTE) * 5);

	_client_socket->Receive_Data((char *)&header_data[0], sizeof(BYTE), received_data_size);

	if (header_data[0] == magic_word[0]) {
		_client_socket->Receive_Data((char *)&header_data[1], sizeof(BYTE), received_data_size);
		if (header_data[1] == magic_word[1]) {
			_client_socket->Receive_Data((char *)&header_data[2], sizeof(BYTE), received_data_size);
			if (header_data[2] == magic_word[2]) {
				_client_socket->Receive_Data((char *)&header_data[3], sizeof(BYTE), received_data_size);
				if (header_data[3] == magic_word[3]) {
					_client_socket->Receive_Data((char *)&header_data[4], sizeof(BYTE), received_data_size);
					if (header_data[4] == magic_word[4]) {// get all Header data 
						//Get the image size
						_client_socket->Receive_Data((char *)&_image_data_size, sizeof(BYTE)*4, received_data_size);
						return true;
					}
				}
			}
		}
	}

	return NETWORK_DISCONNECTION;
}