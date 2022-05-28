_cache = {}

# Returns True if entry already exists else False
def addEntry(uuid, anchor, node, data):
    current_val = _cache.get(node)
    if (current_val == None):
        # Entry does not exist
        _cache[node] = dict()
        _cache[node][anchor] = data
        return False
    else:
        # Entry exists -> appending...
        _cache[node][anchor] = data
        return True

def returnEntry(node):
    return _cache.get(node)

def removeEntry(node):
    _cache.pop(node)