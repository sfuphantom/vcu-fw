import itertools
import time
import sys
import serial

BSE_MIN = 1500
BSE_MAX = 4500

APPS1_MIN = 1500
APPS1_MAX = 4500

APPS2_MIN = 500
APPS2_MAX = 1500

def throttle_encode(a1: int=APPS1_MIN, a2: int=APPS2_MIN, bse: int=BSE_MIN, tsal: bool = None, rtds: bool = None) -> bytes:

	# determine if we want to send interrupts
	tsal_interrupt_flag = tsal is not None 
	rtds_interrupt_flag = rtds is not None 

	# if we're sending interrupts, add their signal value
	tsal_val = tsal_interrupt_flag << 1 | bool(tsal)
	rtds_val = rtds_interrupt_flag << 1 | bool(rtds)

	assert a1 in range(APPS1_MIN, APPS1_MAX+1)  and a2 in range(APPS2_MIN, APPS2_MAX+1), 'Outside of range'
	value = (a1-APPS1_MIN) | (a2-APPS2_MIN) << 12 | (bse-BSE_MIN) << 22 | tsal_val << 34 | rtds_val << 36  # shift range to zero 
	return value.to_bytes(5, byteorder='little')

def test_throttle_output(delay_ms=33, step=20): # 33 ms seems to be our latency

	board = serial.Serial(port=sys.argv[1], baudrate=460800)

	apps1 = range(APPS1_MIN, APPS1_MAX, step)
	apps2 = range(APPS2_MIN, APPS2_MAX, step)
	bse = range(BSE_MIN, BSE_MAX, step)

	print(f"Testing with {delay_ms=}")

	response = ""

	for a1, a2, b in zip(apps1, apps2, bse):

		try:
			message = throttle_encode(a1, a2, b, rtds=False) 
		except AssertionError:
			break

		board.write(message)
		time.sleep(delay_ms/1000)
		response += board.read_all().decode()


	board.close()

	for a1, a2, b in zip(apps1, apps2, bse):
		assert "{} {} {}".format(a1, a2, b) in response, response + f"\nCheck failed ({delay_ms=}): {a1} {a2} {b}"

	return response + "Test passed!"

def test_latency():

	for i in itertools.cycle(range(35, 0, -1)): # 35 ms, step down by 1ms
		test_throttle_output(i)

def test_interrupt_output():

	ret = ''
	board = serial.Serial(port=sys.argv[1], baudrate=460800)

	board.write(throttle_encode(rtds=0, tsal=0))
	time.sleep(1)
	response = board.read_all().decode() 
	assert "EVENT_TRACTIVE_OFF" in response, response
	ret += response

	board.write(throttle_encode(rtds=1, tsal=1))
	time.sleep(1)
	response = board.read_all().decode() 
	assert "EVENT_TRACTIVE_ON" in response and "Ready to drive!" in response, response
	ret += response

	board.write(throttle_encode())
	time.sleep(1)
	response = board.read_all().decode() 
	assert all(r not in response for r in ["EVENT_TRACTIVE_ON", "EVENT_TRACTIVE_OFF", "Ready to drive!"]), response
	ret += response

	board.close()

	return ret + "Test passed!"
	


if __name__ == '__main__':
	print(test_interrupt_output())
	print(test_throttle_output())
	# test_latency()
