import time
from threading import Thread

import data_cache
import loc_algorithm


class run(Thread):
    def __init__(self, uuid):
        Thread.__init__(self)
        self._uuid = uuid

    def run(self):
        time.sleep(1)
        node = 1
        loc_algorithm.run(node, data_cache.returnEntry(node))

        # Free up uuid
        print("Done")
        data_cache.removeEntry(node)