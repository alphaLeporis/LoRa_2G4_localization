import redis
import time
import json
import threading
import os
from dotenv import load_dotenv

import anchor_cache
import data_cache
import data_worker

load_dotenv()

r = redis.Redis(host=os.getenv('REDIS_IP'), port=int(os.getenv('REDIS_PORT')), db=0, decode_responses=True, password=os.getenv('REDIS_AUTH'))
p = r.pubsub()
p.subscribe("loc-data")

anchor_cache.updateAnchors()

while True:
    message = p.get_message()
    if message and (not message["type"] == 'subscribe'):
        print(message)
        y = json.loads(str(message["data"]))
        exists = data_cache.addEntry(y["id"], y["anchor"], y["node"], y["data"])
        if (not exists):
            print("Thread started")
            data_worker.run(y["id"]).start()
            print("goes on...")