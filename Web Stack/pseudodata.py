import sys
import httplib
import random
import urllib

def pseudodata(args, appliance_dict):
	if len(args) < 3:
		print "Usage: "+sys.argv[0]+" device start stop"
		sys.exit()

	conn = httplib.HTTPConnection("seads.brabsmit.com")
	start = int(args[1])
	stop = int(args[2])
	duration = stop - start
	for i in range(duration):
		timestamp = start + i
		for appliance, seed in appliance_dict.iteritems():
			wattage = float(seed) - random.randrange(-10, 10)
			payload = '{"device":"'+args[0]+'",'+\
					   '"appliance:":"'+appliance+'",'+\
					   '"timestamp":"'+str(timestamp)+'",'+\
					   '"wattage":"'+str(wattage)+'"}'
			print payload
			header = {"Content-Type": "application/json",
					  "Authorization": "Token 0d1e0f4b56e4772fdb440abf66da8e2c1df799c0"}

			conn.request("POST", "/api/event-api/", payload, header)
			response = conn.getresponse()
			print response.status, response.reason
			print response.read()
	conn.close()


if __name__ == "__main__":
	appliance_dict = {}
	i=1
	while True:
		new_appliance = raw_input("Appliance "+str(i)+":")
		if len(new_appliance) == 0: break
		average_power = raw_input("Average Wattage:" )
		appliance_dict[new_appliance] = average_power
		i += 1
	if len(appliance_dict) == 0:
		appliance_dict['/api/appliance-api/1/'] = "100"
	pseudodata(sys.argv[1:], appliance_dict)