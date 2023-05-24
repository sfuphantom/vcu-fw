
# typedef struct SerialIntData_t{
# 	uint8_t intflag       : 1;
# 	uint8_t tsal_int_flag : 1;
# 	uint8_t rtds_int_flag : 1;
# 	uint8_t tsal_value    : 1;
# 	uint8_t rtds_value    : 1;
# 	uint8_t padding       : 3;
# }SerialIntData_t;

def interrupt_encode(tsal: bool = None, rtds: bool = None) -> bytes:

	# determine if we want to send interrupts
    tsal_interrupt_flag = tsal is not None 
    rtds_interrupt_flag = rtds is not None 

    value = (0b1 << 7) | (tsal_interrupt_flag << 6) | (rtds_interrupt_flag << 5) 

	# if we're sending interrupts, add their signal value
    if tsal_interrupt_flag:
        value |= (tsal << 4 ) 

    if rtds_interrupt_flag:      
        value |= (rtds << 3 )   

    return value.to_bytes(1, byteorder='little')


print(bin( int(interrupt_encode(rtds=0, tsal=0).hex(), base=16) ))


