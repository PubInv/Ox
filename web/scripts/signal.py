import requests
from datetime import datetime
import json
import time
import math

current_time = datetime.now().isoformat()
print(current_time)

URL = "http://localhost:3020/pimd"

i = 0
while i < 1000:
    payload = {
        "location": "D",
        "value": 10*math.sin(i), #math.ceil(10*math.sin(i)),
        "time": datetime.now().isoformat()
    }

    print(payload)
    requests.post(url = URL, data = payload)
    i = i + 0.1
    time.sleep(1)