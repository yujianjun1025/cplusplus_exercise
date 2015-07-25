#coding=utf-8
import MySQLdb
import os
import sys

reload(sys)
sys.setdefaultencoding("utf-8")

db = MySQLdb.connect(user='meilv',db='meilv',passwd='b@+fmV80%5W]BB',host='10.64.35.8',port=5002,charset='utf8')
cursor = db.cursor()

if __name__=="__main__":
    sql_str='''
select
  p.mdc_poi_name, group_concat(distinct concat(c.name, ':', c.city_id))
from
  trip_poi_info p
  join city_all_info l on p.city_location_id=l.location_id 
  join city_info c on l.city_id=c.city_id
where p.mdc_poi_name !='' and p.isdel=0 and p.status=0  
group by p.mdc_poi_name
'''

   cursor.execute(sql_str);
   while(True):
       data = cursor.fetchone()
       if( data != None):
           if data[0] == '西藏':
               continue
           print data[0].replace('\t', '') + '\t' + data[1]
       else:
           break


