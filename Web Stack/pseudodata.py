import sys
import httplib2
import random

class Appliance:
	def __init__(self, name, averagepower):
		self.name = name
		self.averagepower = averagepower

class C:
	def __init__(self, header, device, time_start, time_stop, appliance, power, conn):
		self.header = header
		self.device = device
		self.time_start = time_start
		self.time_stop = time_stop
		self.appliance = appliance
		self.power = power
		self.conn = conn

def compute(obj):
	for i in range (obj.time_stop - obj.time_start):
                power = random.uniform(-.25*obj.power, .25*obj.power) + obj.power
		payload = '{"device":"'+obj.device+'",'+\
				   '"appliance":"'+obj.appliance+'",'+\
				   '"timestamp":"'+str(i)+'",'+\
				   '"wattage":"'+str(power)+'"}'

		resp, content = obj.conn.request("http://seads.brabsmit.com/api/event-api/","POST", payload, obj.header)
                if resp['status'] != '201':
                   print resp
                   print content
                   sys.exit(1)

def get_appliances():
	appliances = []
	i=1
	while True:
		new_appliance = raw_input("Appliance pk"+str(i)+":")
		if len(new_appliance) == 0: break
		averagepower = raw_input("Average Wattage:" )
		appliance = Appliance("/api/device-api/"+new_appliance+"/", averagepower)
		appliances.append(appliance)
		i += 1

	if len(appliances) == 0:
		appliances.append(Appliance('/api/appliance-api/5/', '700'))

	return appliances

if __name__ == "__main__":
	args = sys.argv[1:]
	if len(args) < 3:
		print "Usage: "+sys.argv[0]+" device start stop"
		sys.exit()

	device = sys.argv[1]
	appliances = get_appliances()
	time_start = int(sys.argv[2])
	time_stop = int(sys.argv[3])
	conn = httplib2.Http()
	header = {"Content-Type": "application/json"}
	jobs = []
	for appliance in appliances:
		power = int(appliance.averagepower)
		c = C(header, device, time_start, time_stop, appliance.name, power, conn)
		compute(c)
