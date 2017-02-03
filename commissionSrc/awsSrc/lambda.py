from __future__ import print_function

import logging, json
import boto3

# enable basic logging to CloudWatch Logs
logger = logging.getLogger()
logger.setLevel(logging.INFO)

# setup the DynamoDB table
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('alexa-creds')


def lambda_handler(event, context):

    deviceID = event['deviceID']
    amazonEmail = event['amznEmail']
    fanDeviceName = event['fanDeviceName']
    lightDeviceName = event['lightDeviceName']

    table.put_item(
        Item={
            'deviceID' : deviceID,
            'amznEmail' : amazonEmail,
            'fanDeviceName' : fanDeviceName,
            'lightDeviceName' : lightDeviceName
        }
    )
    print(event)
    return('Success!')
