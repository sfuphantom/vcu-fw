import sys
from vcu_communication import VCU_Communication
from vcu_communication import (
	APPS1_MIN,
	APPS1_MAX,
	APPS2_MIN,
	APPS2_MAX,
	BSE_MIN,
	BSE_MAX
)


def test_throttle_output(step=20): 

	vcu = VCU_Communication(sys.argv[1], logger=print)

	apps1 = range(APPS1_MIN, APPS1_MAX, step)
	apps2 = range(APPS2_MIN, APPS2_MAX, step)
	bse = range(BSE_MIN, BSE_MAX, step)

	response = ""

	for a1, a2, b in zip(apps1, apps2, bse):

		response += vcu.write(a1, a2, b)


	for a1, a2, b in zip(apps1, apps2, bse):
		assert "{} {} {}".format(a1, a2, b) in response, response + f"\nCheck failed: {a1} {a2} {b}"
	
	# check that values don't change if nothing provided
	assert "{} {} {}".format(vcu.apps1, vcu.apps2, vcu.bse) in vcu.write(delay_s=5), response + "\nCheck failed: {} {} {}".format(vcu.apps1, vcu.apps2, vcu.bse)

	print(f"{test_throttle_output.__name__} passed!")
	return response + "Test passed!"

def test_interrupt_output():

	ret = ''

	vcu = VCU_Communication(sys.argv[1], logger=print)

	response = vcu.write(rtds_flip=True, tsal_flip=True, delay_s=0.5)
	assert "EVENT_TRACTIVE_ON" in response and "Ready to drive!" in response, response
	ret += response

	response = vcu.write(rtds_flip=True, tsal_flip=True, delay_s=0.5)
	assert "EVENT_TRACTIVE_OFF" in response, response
	ret += response

	response = vcu.write(set_reset_flip=True, delay_s=0.5)
	assert "SET" in response, response
	ret += response

	response = vcu.write(set_reset_flip=True, delay_s=0.5)
	assert "RESET" in response, response
	ret += response

	response = vcu.write(set_reset_flip=True, delay_s=0.5)
	assert "SET" in response, response
	ret += response

	response = vcu.write(delay_s=0.5)
	assert all(r not in response for r in ["SET", "RESET", "EVENT_TRACTIVE_ON", "EVENT_TRACTIVE_OFF", "Ready to drive!"]), response
	ret += response

	print(f"{test_interrupt_output.__name__} passed!")
	return ret 

if __name__ == '__main__':
	# test_interrupt_output()
	test_throttle_output()
	# test_latency()
