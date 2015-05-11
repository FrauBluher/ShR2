from django.core.management.base import BaseCommand, CommandError
from microdata.models import Device
from django.conf import settings
from influxdb.influxdb08 import client as influxdb
import boto3
import json
from datetime import datetime
import os
from treehash import TreeHash

def safe_list_get (l, idx, default):
   try:
      return l[idx]
   except IndexError:
      return default


class Command(BaseCommand):
   args = 'begin end resolution'
   help = 'Specify a range (timestamps) of data to export to the S3 volume with the given resolution'

   def handle(self, *args, **options):
      start = safe_list_get(args, 0, 0)
      end = safe_list_get(args, 1, 0)
      resolution = safe_list_get(args, 2, 1)
      part_size = 8388608
      
      print 'Contacting Amazon AWS...'
      glacier = boto3.client('glacier')
      multipart_upload = glacier.initiate_multipart_upload(vaultName=settings.GLACIER_VAULT_NAME, partSize=str(part_size))
      print 'Connected to Glacier Vault "'+settings.GLACIER_VAULT_NAME+'"'
      upload_id = multipart_upload['uploadId']
      treehash_archive = TreeHash()
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      
      archive_size = 0
      for device in Device.objects.all():
         series = 'device.'+str(device.serial)
         try:
            query = 'select * from '+series+' where time > '+str(start)+'ms and time < '+str(end)+'ms'
            points = db.query(query)
         except:
            print series+' not found. Skipping.'
            continue
         print "Uploading "+series+"..."
         print "Querying from "+str(datetime.fromtimestamp(int(start)/1000))+" to "+str(datetime.fromtimestamp(int(end)/1000))
         # store points in temporary file, break into 8MB parts
         with open('/tmp/temp_archive','wb') as f:
            f.write(json.dumps(points))
         bytes_read = 0
         bytes_sent = 0
         # compute hash
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
         #db.query('delete from '+series+' where time > '+str(start)+'s and time < '+str(end)+'s')
         print "[DONE]"
      with open('/tmp/temp_archive','rb') as f:
         treehash_archive.update(f.read())
      response = glacier.complete_multipart_upload(vaultName=settings.GLACIER_VAULT_NAME,
                                                   uploadId=upload_id,
                                                   archiveSize=str(archive_size),
                                                   checksum=treehash_archive.hexdigest())
      os.remove('/tmp/temp_archive')
      print "Archival Successful"