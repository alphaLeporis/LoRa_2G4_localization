

import db_broker
_cache = dict()

def updateAnchors():
    data = db_broker.supabase.table("anchor").select("location","anchor_id").execute()
    assert len(data.data) > 0
    for node in data.data:
        print(node)
        _cache[node['anchor_id']] = node['location']