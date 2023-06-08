import itertools
import time
import sys
import serial
from typing import Iterable

BSE_MIN = 1500
BSE_MAX = 4500

APPS1_MIN = 1500
APPS1_MAX = 4500

APPS2_MIN = 500
APPS2_MAX = 1500

throttle_output_on_startup = ['60', '60', '260', '274', '283', '291', '298', '304', '310', '316', '321', '327', '333', '338', '344', '350', '355', '361', '367', '372', '378', '384', '389', '395', '401', '407', '412', '418', '424', '429', '435', '441', '446', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450']

def throttle_encode(a1: int, a2: int) -> bytes:
	assert a1 in range(APPS1_MIN, APPS1_MAX+1)  and a2 in range(APPS2_MIN, APPS2_MAX+1), 'Outside of range'
	value = (a1-APPS1_MIN) | (a2-APPS2_MIN) << 12 # shift to range to zero 
	return value.to_bytes(3, byteorder='little')

def latency_checker(a1, a2, response, sleep_ms):
	print(f'{sleep_ms*1000=}')
	print(f"{response=}")
	assert str(a1) in response, f'{a1=} not in {response=}'
	assert str(a2) in response, f'{a2=} not in {response=}'

def test_throttle_output(i=320, latency_checker=lambda *args: None):

	board = serial.Serial(port=sys.argv[1], baudrate=460800)

	apps1 = range(APPS1_MIN, APPS1_MAX, 20)
	apps2 = range(APPS2_MIN, APPS2_MAX, 20)
	
	response = ""
	for a1, a2 in zip(apps1, apps2):
		try:
			message = throttle_encode(a1, a2) 
		except AssertionError:
			break

		board.write(message)


		time.sleep(32/1000)
		response += board.read_all().decode()



	board.close()

	for a1, a2 in zip(apps1, apps2):

		assert "{} {}".format(a1, a2) in response, response

	return response + "Test passed!"

def test_latency():

	for i in itertools.cycle(range(320, 0, -1)): # 32 ms, step down by 1ms
		
		test_throttle_output(i, latency_checker)




def send_output(apps1: Iterable, apps2: Iterable):

	board = serial.Serial(port=sys.argv[1])

	n = 0
	t0 = round(time.time() * 1000)

	response = []

	input_signal = []

	try:
		for a1, a2 in itertools.cycle(zip(apps1, apps2)):

			try:
				message = throttle_encode(a1, a2) 
			except AssertionError:
				break

			data_pt = (round(time.time() * 1000)-t0, a1, a2)

			input_signal.append(data_pt)
			board.write(message)

			time.sleep(0.035)
			pt = (round(time.time() * 1000)-t0, board.read_all().decode())
			print(pt, data_pt)
			response.append(pt)

			n += 1

	except KeyboardInterrupt:
		pass

	finally:

		board.close()

		return response


if __name__ == '__main__':
	for _ in range(10):
		print(test_throttle_output())

	
	# print(send_output(APPS_RANGES[0], APPS_RANGES[1]))