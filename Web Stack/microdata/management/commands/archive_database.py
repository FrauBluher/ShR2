from django.core.management.base import BaseCommand, CommandError
from microdata.models import Device
from django.conf import settings
from influxdb.influxdb08 import client as influxdb
import boto3
import json
from datetime import datetime, timedelta
import os
from treehash import TreeHash
import time
import re

def safe_list_get (l, idx, default):
   try:
      return l[idx]
   except IndexError:
      return default


class Command(BaseCommand):
   args = ''
   help = 'Backs up the data for each device relative to its retention policy.'

   def handle(self, *args, **options):
      part_size = 8388608
      
      print 'Contacting Amazon AWS...'
      glacier = boto3.client('glacier')
      multipart_upload = glacier.initiate_multipart_upload(vaultName=settings.GLACIER_VAULT_NAME, partSize=str(part_size))
      print 'Connected to Glacier Vault "'+settings.GLACIER_VAULT_NAME+'"'
      upload_id = multipart_upload['uploadId']
      treehash_archive = TreeHash()
      db = influxdb.InfluxDBClient(settings.INFLUXDB_URI,8086,'root','root','seads')
      
      archive_size = 0
      for device in Device.objects.all():
         start = datetime.fromtimestamp(0)
         end = datetime.now() - timedelta(days=31*device.data_retention_policy)
         start = (datetime.now() - start).total_seconds()
         start = 0
         end = int((datetime.now() - end).total_seconds())
         end = time.time() - end
         print 'Trying '+str(device)+'...'
         print 'Data Retention Policy: '+str(device.data_retention_policy)+' Months'
         series = 'device.'+str(device.serial)
         try:
            query = 'select * from '+series+' where time > '+str(start)+'s and time < '+str(end)+'s'
            points = db.query(query)
         except:
            print 'No data found for '+series+'. Skipping.'
            continue
         print "Uploading "+series+"..."
         print "Querying from "+str(datetime.fromtimestamp(int(start)))+" to "+str(datetime.fromtimestamp(int(end)))
         # store points in temporary file, break into 8MB parts
         with open('/tmp/temp_archive','wb') as f:
            f.write(json.dumps(points))
         bytes_read = 0
         bytes_sent = 0
         with open('/tmp/temp_archive','rb') as f:
            treehash_part = TreeHash()
            part = f.read(part_size)
            treehash_part.update(part)
            bytes_read += len(part)
            while part:
               response = glacier.upload_multipart_part(vaultName=settings.GLACIER_VAULT_NAME,
                                                        uploadId=upload_id,
                                                        range='bytes '+str(bytes_sent)+'-'+str(bytes_read-1)+'/*',
                                                        body=part,
                                                        checksum=treehash_part.hexdigest())
               bytes_sent += len(part)
               part = f.read(part_size)
               treehash_part.update(part)
               bytes_read += len(part)
         archive_size += 1
         print "Successfully uploaded "+str(bytes_sent)+" bytes to Glacier"
         print "Deleting points from database..."
         # drop from fanout series as well
         series = db.query('list series')[0]['points']
         rg = re.compile('device.'+str(device.serial))
         for s in series:
            if rg.search(s[1]):
               db.query('delete from '+s[1]+' where time > '+str(start)+'s and time < '+str(end)+'s')
         print "[DONE]"
      try:
         with open('/tmp/temp_archive','rb') as f:
            treehash_archive.update(f.read())
         response = glacier.complete_multipart_upload(vaultName=settings.GLACIER_VAULT_NAME,
                                                      uploadId=upload_id,
                                                      archiveSize=str(archive_size),
                                                      checksum=treehash_archive.hexdigest())
         with open(settings.STATIC_PATH+'archive_ids.log','a') as f:
            line = {'archiveId':response['archiveId'], 'timeEnd':str(end)}
            f.write(json.dumps(line))
            f.write(';')
         os.remove('/tmp/temp_archive')
         print "Archival Successful"
      except:
         print "No data to archive. Exiting."