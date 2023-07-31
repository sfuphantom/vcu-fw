import time
import serial

from StatesAndEvents import ResponseVCU

BSE_MIN = 1500
BSE_MAX = 4500

APPS1_MIN = 1500
APPS1_MAX = 4500

APPS2_MIN = 500
APPS2_MAX = 1500

class VCUSimulation:

	BAUDRATE: int = 460800

	_port: str
	_rtds: bool = False
	_tsal: bool = False

	_apps1: int = APPS1_MIN
	_apps2: int = APPS2_MIN
	_bse: int = BSE_MIN

	@property
	def apps1(self):
		return self._apps1

	@property
	def apps2(self):
		return self._apps2

	@property
	def bse(self):
		return self._bse

	@property
	def tsal(self):
		return self._tsal

	@property
	def rtds(self):
		return self._rtds

	@property
	def __tsal_flip(self):
		self._tsal = not self.tsal
		return self.tsal

	@property
	def __rtds_flip(self):
		self._rtds = not self.rtds
		return self.rtds

	def __init__(self, port: str, logger=lambda x: None) -> None:
		self._port = port

		self.get_port = lambda: serial.Serial(self._port, baudrate=VCUSimulation.BAUDRATE)

		# test the port
		with self.get_port():
			pass

		self._logger = logger

		self._logger("Initializing logger...")

	def __encode(self, a1: int, a2: int, bse: int, tsal_flip: bool, rtds_flip: bool) -> bytes:

		# update internal state values
		self._apps1 = a1
		self._apps2 = a2
		self._bse = bse

		# if we're sending interrupts, add their signal value
		tsal_val = tsal_flip << 1 | self.__tsal_flip if tsal_flip else self.tsal
		rtds_val = rtds_flip << 1 | self.__rtds_flip if rtds_flip else self.rtds

		assert a1 in range(APPS1_MIN, APPS1_MAX+1)  and a2 in range(APPS2_MIN, APPS2_MAX+1), 'Outside of range'
		value = (a1-APPS1_MIN) | (a2-APPS2_MIN) << 12 | (bse-BSE_MIN) << 22 | tsal_val << 34 | rtds_val << 36  # shift range to zero 
		return value.to_bytes(5, byteorder='little')

	def write(self, a1: int=None, a2: int=None, bse: int=None, 
	   		  tsal_flip: bool=False, rtds_flip: bool=False, delay_s: int=0) -> ResponseVCU:
		
		"""
		:return: data structure containing the events and state changes
		"""
		# use cached values if none provided
		a1 = self._apps1 if a1 is None else a1
		a2 = self._apps2 if a2 is None else a2
		bse = self._bse if bse is None else bse
		
		message = self.__encode(a1, a2, bse, tsal_flip, rtds_flip)

		self._logger(f"Sending {message}...")

		with self.get_port() as vcu:
			vcu.write(message)
			time.sleep(33/1000) # best latency we can get. anything less is not guranteed to receive 

			time.sleep(delay_s/1000) # add additional user delay. this point is when the low priority tasks are done executing

			ret = vcu.read_all().decode()
			self._logger(ret)
			return ret
	