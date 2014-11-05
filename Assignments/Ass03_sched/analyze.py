import json
from pprint import pprint

types = ["cpu_", "mix_", "io_"]
sched = ["normal_", "fifo_", "rr_"]
load = ["low", "medium", "high"]

cpu_files = []
io_files = []
mix_files = []

for b in sched:
	for c in load:
		cpu_files.append("test/cpu_" + b + c + ".txt")
		io_files.append("test/io_" + b + c + ".txt")
		mix_files.append("test/mix_" + b + c + ".txt")
		

cpu_times = {}
io_times = {}
mix_times = {}
for i in cpu_files:
	info = json.loads(open(i).readlines()[0])
	cpu_times.update({i:info['time']})
	
for i in io_files:
	info = json.loads(open(i).readlines()[0])
	io_times.update({i:info['time']})
	
for i in mix_files:
	info = json.loads(open(i).readlines()[0])
	mix_times.update({i:info['time']})


report = {}
report.update({"Results": { "cpu":cpu_times, "io":io_times, "mix":mix_times}})
pprint (report)
