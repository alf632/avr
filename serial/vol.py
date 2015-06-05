#/usr/bin/python

import cherrypy
import serial
import re
import sys
from time import sleep
from jinja2 import Environment, FileSystemLoader
env = Environment(loader=FileSystemLoader('templates'))

port = "/dev/ttyUSB1"
ser = serial.Serial(port, 1200, timeout=10)

class Root(object):
   
    @cherrypy.expose
    def index(self):
	self.ser=ser
	tmpl = env.get_template('index.html')
	target=self.read_vol()[1]
	print "target: "+target
	return tmpl.render(target=str(target))

    def read_vol(self):
        self.ser.write("i")
        sleep(1)
        vol=""
        target=""
        data=""
        while self.ser.inWaiting() > 0:
            data = data + self.ser.read(1)
	print "serial "+str(data)
    	target_regex=re.search("Target:\d+\\n", data, flags=0)
    	vol_regex=re.search("Vol:\d+", data, flags=0)
    	if target_regex:
    	    target=target_regex.group(0).replace("Target:", "").replace("\n", "")
    	    vol=vol_regex.group(0).replace("Vol:","")
        return vol,target

    @cherrypy.expose
    def write_target(self, **data):
        self.ser.write("t")
        sleep(0.5)
	target=data['target']
	print "data_target: "+target
        target=chr(int(target))
        self.ser.write(target)
	tmpl = env.get_template('index.html')
        print "new target: "+self.read_vol()[1]
        return tmpl.render(target="target")
    
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
#    main()
#    cherrypy.config.update({'server.socket_host': '127.0.0.1',
#                            'server.socket_port': 8080,
#                          })
    cherrypy.quickstart(Root())
    ser.close()
