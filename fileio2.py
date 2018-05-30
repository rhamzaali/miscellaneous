from __future__ import print_function
import re
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
import pandas as pd
import geopy.distance

p =  re.compile("(\[null,null,)+\-?[0-9]+\.?[0-9]*(,)+\-?[0-9]+\.?[0-9]*(\])")

def distSub(row):
    return geopy.distance.vincenty((row['subLat_x'],row['subLong_x']),(row['compLat'],row['compLong'])).m
def compDistSchool(row):
    return geopy.distance.vincenty((row['schoolLat'],row['schoolLong']),(row['compLat'],row['compLong'])).m
def compDistGrocery(row):
    return geopy.distance.vincenty((row['groceryLat'],row['groceryLong']),(row['compLat'],row['compLong'])).m
def compDistRecreation(row):
    return geopy.distance.vincenty((row['recreationLat'],row['recreationLong']),(row['compLat'],row['compLong'])).m
def subDistSchool(row):
    return geopy.distance.vincenty((row['schoolLat'],row['schoolLong']),(row['subLat_x'],row['subLong_x'])).m
def subDistGrocery(row):
    return geopy.distance.vincenty((row['groceryLat'],row['groceryLong']),(row['subLat_x'],row['subLong_x'])).m
def subDistRecreation(row):
    return geopy.distance.vincenty((row['recreationLat'],row['recreationLong']),(row['subLat_x'],row['subLong_x'])).m

def extract(a):
    m = re.search(p,a).group(0)
    n = a[a.index(m)+len(m):a.index(m)+len(m)+100].split(',')[2].replace('\"','').replace('\\','')
    m = m.replace('[','').replace(']','').split(',')
    return n +','+str(m[2])+','+str(m[3])

# generates df with unique subjects (guID,lat,long)
# could be written in SQL/R
out = open("output.txt",'w')

subj = pd.DataFrame(columns = ["guID", "subLat", "subLong", "school","schoolLat","schoolLong","grocery",
                "groceryLat","groceryLong","recreation","recreationLat","recreationLong"])


data = pd.read_csv('testfile2.txt', sep=",", header=None)
data.columns = ["guID", "date", "val", "subLat","subLong","rank","compLat","compLong"]

data2 = pd.concat([data['guID'],data['subLat'],data['subLong']],axis=1,keys=['guID','subLat','subLong'])
data2 =  data2.drop_duplicates('guID')
#print(data2)


driver = webdriver.Chrome()

for index,row in data2.iterrows():
    # find school
    final =  row['guID']+','+str(row['subLat'])+','+str(row['subLong'])+','
    driver.get('https://www.google.com/maps/search/school/@'+str(row['subLat'])+','+str(row['subLong'])+',13z')
    a = driver.page_source
    final += extract(a)+','

    # find grocery
    driver.get('https://www.google.com/maps/search/grocery/@'+str(row['subLat'])+','+str(row['subLong'])+',13z')
    a = driver.page_source
    a = driver.page_source
    final += extract(a)+','

    # find recreation
    driver.get('https://www.google.com/maps/search/recreation/@'+str(row['subLat'])+','+str(row['subLong'])+',13z')
    # content = driver.find_element_by_class_name('section-result-header').find_element_by_tag_name("h3").find_element_by_tag_name("span").get_attribute("innerHTML")
    a = driver.page_source
    final += extract(a)
    
    #print(final)
    #print(final,file=out)
    final = final.split(",")
    subj.loc[len(subj)] = final



combined = pd.merge(data,subj,on='guID')

distances = combined.drop('date',1). \
            drop('val',1).drop('rank',1). \
            drop('subLat_y',1). \
            drop('subLong_y',1). \
            drop("school",1). \
            drop("grocery",1).  \
            drop("recreation",1)


distances['sub_distSchool'] = distances.apply(subDistSchool, axis=1)
distances['sub_distGrocery'] = distances.apply(subDistGrocery, axis=1)
distances['sub_distRecreation'] = distances.apply(subDistRecreation, axis=1)

distances['comp_distSubject'] = distances.apply(distSub, axis=1)
distances['comp_distSchool'] = distances.apply(compDistSchool, axis=1)
distances['comp_distGrocery'] = distances.apply(compDistGrocery, axis=1)
distances['comp_distRecreation'] = distances.apply(compDistRecreation, axis=1)


staging = distances.drop('schoolLat',1). \
            drop('schoolLong',1).drop('groceryLat',1). \
            drop('groceryLong',1). \
            drop("recreationLat",1). \
            drop("recreationLong",1)

print(staging)
staging.to_csv(r'staging.txt', header=None, index=None, sep=',', mode='a')

driver.close()
    
out.close()

#tor_process.kill()
