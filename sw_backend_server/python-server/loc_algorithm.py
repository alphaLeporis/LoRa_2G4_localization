import statistics
import localization as lx
import numpy as num
from scipy.optimize import minimize, fmin_cobyla
import shapely as shx
import numpy as np

import anchor_cache
import db_broker


def run(id, all_data):
    if (len(all_data) < 3):
        print(all_data)
        print("Not enough data")
        return
    cleaned_data = remove_outliers_mean(all_data)
    loc_data = compute(cleaned_data)
    saveLocData(id, loc_data)



def remove_outliers_mean(all_data):
    cleaned_data = dict()
    outlierConstant = 1
    print(all_data)
    for anchor in all_data:
        a = np.array(all_data[anchor])
        print(a)
        upper_quartile = np.percentile(a, 75)
        lower_quartile = np.percentile(a, 25)
        IQR = (upper_quartile - lower_quartile) * outlierConstant
        quartileSet = (lower_quartile - IQR, upper_quartile + IQR)
        resultList = []
        for y in a.tolist():
            if y >= quartileSet[0] and y <= quartileSet[1]:
                resultList.append(y)
        cleaned_data[anchor] = statistics.mean(resultList)
    return cleaned_data

def compute(all_data):
    P = lx.Project(mode='3D', solver='LSE')
    for anchor in all_data.keys():
        P.add_anchor(anchor, (anchor_cache._cache[anchor]['x'],
                              anchor_cache._cache[anchor]['y'],
                              anchor_cache._cache[anchor]['z']))

    t, label = P.add_target()
    for measurement in all_data:
        t.add_measure(measurement, all_data[measurement])

    P.solve()

    # Then the target location is:
    loc = dict()
    loc['x'] = round(t.loc.x)
    loc['y'] = round(t.loc.y)
    loc['z'] = round(t.loc.z)
    print(loc)
    return loc

def saveLocData(node, loc_data):
    data = db_broker.supabase.table("distance_reading").insert({"node": node, "location": loc_data}).execute()
    assert len(data.data) > 0