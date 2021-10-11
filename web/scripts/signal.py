import requests
from datetime import datetime
import json

current_time = datetime.now().isoformat()
print(current_time)

URL = "http://localhost:3020/"

payload = {
    "location": "C",
    "value": 12345,
    "time": datetime.now().isoformat()
}

print(payload)
requests.post(url = URL, data = payload)