import itertools
import time
import sys
import serial
from typing import Iterable

BSE_RANGE   = range( 1500, 4500  + 1 )
APPS_RANGES = [ range( 1500, 4500  + 1 ), range( 500,  1500  + 1 ) ]

throttle_output_on_startup = ['60', '60', '260', '274', '283', '291', '298', '304', '310', '316', '321', '327', '333', '338', '344', '350', '355', '361', '367', '372', '378', '384', '389', '395', '401', '407', '412', '418', '424', '429', '435', '441', '446', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450', '450']

def throttle_encode(a1: int, a2: int) -> bytes:
    assert a1 in APPS_RANGES[0] and a2 in APPS_RANGES[1], 'Outside of range'
    value = (a1-min(APPS_RANGES[0])) | (a2-min(APPS_RANGES[1])) << 12
    return value.to_bytes(3, byteorder='little')

def latency_checker(a1, a2, response, sleep_ms):
    print(f'{sleep_ms*1000=}')
    print(f"{response=}")
    assert str(a1) in response, f'{a1=} not in {response=}'
    assert str(a2) in response, f'{a2=} not in {response=}'

def test_throttle_output(i=320, latency_checker=lambda *args: None):

    ret = []
    board = serial.Serial(port=sys.argv[1], baudrate=460800)

    n = 0
    while True:
        a1 = (20*n)+ 1500
        a2 = (20*n) + 500

        try:
            message = throttle_encode(a1, a2) 
        except AssertionError:
            break

        board.write(message)

        sleep_ms = i/10000	

        time.sleep(sleep_ms)
        response = board.read_all().decode()

        ret.append(response)
        assert f'throttle={throttle_output_on_startup[n]}' in response, f'throttle={throttle_output_on_startup[n]} not in {response=}' 
        
        latency_checker(a1, a2, response)

        n += 1

    board.close()

    print('Test passed!')

    return ret

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
    print('\n'.join(test_throttle_output()))
    
    # print(send_output(APPS_RANGES[0], APPS_RANGES[1]))