import requests
from datetime import datetime
import json
import time
import math

##################################################
# Simple mock signal generator script to post    #
# data to the api endpoint                       #
##################################################

URL = "http://localhost:3020/api/pimd"

# Equivalent ISO time formate to JavaScript
def iso_format_js(dt):
    isostring = datetime.strftime(dt, '%Y-%m-%dT%H:%M:%S.{0}Z')
    return isostring.format(int(round(dt.microsecond/1000.0)))

i = 0
while i < 1000:
    payload = {
        "location": "D",
        "value": 10*math.sin(i),
        "time": iso_format_js(datetime.utcnow())
    }

    print(payload)
    requests.post(url = URL, data = payload)
    i = i + 0.1
    time.sleep(1)