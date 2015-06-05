#/usr/bin/python

import serial
import re
import sys
from time import sleep

port = "/dev/ttyUSB0"
ser = serial.Serial(port, 1200, timeout=10)

def read_vol():
    ser.write("i")
    sleep(1)
    vol=""
    target=""
    data=""
    while ser.inWaiting() > 0:
        data = data + ser.read(1)
	target_regex=re.search("Target:\d+\\n", data, flags=0)
	vol_regex=re.search("Vol:\d+", data, flags=0)
	if target_regex:
	    target=target_regex.group(0).replace("Target:", "").replace("\n", "")
	    vol=vol_regex.group(0).replace("Vol:","")
    return vol,target

def write_target(target="0"):
    ser.write("t")
    sleep(0.3)
    target=chr(int(target))
    ser.write(target)
    return target

def main():   
    try:
	sys.argv[1]
    except: #if argv not set
	vol=read_vol()
	target=vol[1]
	vol=vol[0]
	print "vol:%s target:%s" %(vol, target)
	return
    
    #argv present
    print "new Target:"+str(write_target(sys.argv[1]))

if __name__ == "__main__":
    main()
    ser.close()
