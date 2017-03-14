## Copyright (c) 2016 FirstBuild
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


from __future__ import print_function

import logging, json
import boto3, os
from boto3.dynamodb.conditions import Key

# enable basic logging to CloudWatch Logs
logger = logging.getLogger()
logger.setLevel(logging.INFO)

# setup the DynamoDB table
dynamodb = boto3.resource('dynamodb')
crdDB = os.environ['credentialDatabase']
table = dynamodb.Table(crdDB)

def lambda_handler(event, context):
    emailsInDatabase = getExistingDeviceCredentials(event['deviceID'])
    if (emailsInDatabase):
        deleteOldDeviceCredentials(emailsInDatabase)
    storeDeviceCredentials(event)
    return('Success!')

def storeDeviceCredentials(event):
    print('Storing device credentials');
    deviceID = event['deviceID']
    amazonEmail = event['amznEmail'].lower()
    fanDeviceName = event['fanDeviceName']
    lightDeviceName = event['lightDeviceName']

    table.put_item(
        Item={
            'device_id' : deviceID,
            'amzn_email' : amazonEmail,
            'fan_device_name' : fanDeviceName,
            'light_device_name' : lightDeviceName
        }
    )

def getExistingDeviceCredentials(deviceID):
    print('Getting device credentials')
    response = table.query(
        IndexName='device_id_index',
        KeyConditionExpression=Key('device_id').eq(deviceID)
    )
    queryResponse = response['Items']
    emailList = []
    for email in queryResponse:
        emailList.append(email['amzn_email'])
    print(emailList)
    return emailList

def deleteOldDeviceCredentials(emailAddresses):
    print('Eliminating old emails')
    for email in emailAddresses:
        response = table.delete_item(
            Key={'amzn_email' : email}
        )
