from django.core.management.base import BaseCommand, CommandError
from django.contrib.auth.models import User
from django.conf import settings
import boto3



class Command(BaseCommand):
	def handle(self, *args, **options):
		glacier = boto3.client('glacier')
		jobs = glacier.list_jobs(vaultName=settings.GLACIER_VAULT_NAME)['JobList']
		job_count = 0
		if len(jobs) > 0:
			print "Checking "+str(len(jobs))+" job(s)..."
			for job in jobs:
				job_count += 1
				if job['StatusCode'] == 'Succeeded':
					print "Job "+job['JobId']+' ready.'
					# job is ready to be downloaded. Check job description for pk of user requesting the data.
					user = User.objects.get(pk=job['Description'])
					print 'Requester:' + user.username
					ses = boto3.client('ses')
					message = {
						'Subject': {
							'Data': settings.ORG_NAME + ' ' + str(rule).title() + ' Archive Retrieval'
						},
						'Body': {
							'Html': {
								'Data': job.get_output()
							}
						}
					}
					destination = {'ToAddresses': [user.email]}
					ses.send_email(Source=settings.SES_EMAIL,
	                               Destination=destination,
	                               Message=message,
	                               ReturnPath=settings.SES_EMAIL
					)
					print 'Email sent.'
				else:
					print 'Job '+str(job_count)+' not ready. Skipping.'
		else:
			print 'No jobs found. Exiting.'
		print '[DONE]'