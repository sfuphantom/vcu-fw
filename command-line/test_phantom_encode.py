import itertools
import time
import sys
import serial
from typing import Iterable

BSE_RANGE   = range( 1500, 4500  + 1 )
APPS_RANGES = [ range( 1500, 4500  + 1 ), range( 500,  1500  + 1 ) ]

def throttle_encode(a1: int, a2: int) -> bytes:
    assert a1 in APPS_RANGES[0] and a2 in APPS_RANGES[1], 'Outside of range'
    value = (a1-min(APPS_RANGES[0])) | (a2-min(APPS_RANGES[1])) << 12
    return value.to_bytes(3, byteorder='little')

def test_latency():

    board = serial.Serial(port=sys.argv[1])

    for i in itertools.cycle(range(320, 0, -1)): # 32 ms, step down by 1ms
        n = 0
        while True:
            a1 = (1*n)+ 1500
            a2 = (1*n) + 500

            try:
                message = throttle_encode(a1, a2) 
            except AssertionError:
                break

            print(board.write(message))

            sleep_ms = i/10000	
            print(f'{sleep_ms*1000=}')

            time.sleep(sleep_ms)
            response = board.read_all().decode()
            print(f"{response=}")
            assert str(a1) in response
            assert str(a2) in response

            n += 1

    board.close()

def test_throttle_output():

    board = serial.Serial(port=sys.argv[1])

    n = 0
    t0 = round(time.time() * 1000)
    while True:
        a1 = (1*n)+ 1500
        a2 = (1*n) + 500

        try:
            message = throttle_encode(a1, a2) 
        except AssertionError:
            break

        board.write(message)

        time.sleep(0.035)
        response = board.read_all().decode()
        print(round(time.time() * 1000)-t0, response)

        n += 1

    board.close()

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
    # test_latency()
    print(send_output(APPS_RANGES[0], APPS_RANGES[1]))