/** * Copyright (c) 2016 FirstBuild
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
'use-strict';

var https = require('https');
var attr = require('dynamodb-data-types').AttributeValue;
var aws = require('aws-sdk');
var db = new aws.DynamoDB();

const serviceHostName = process.env.SERVICE_HOST_NAME;
const servicePostPath = process.env.SERVICE_POST_PATH;

// namespaces

const NAMESPACE_CONTROL = "Alexa.ConnectedHome.Control";
const NAMESPACE_DISCOVERY = "Alexa.ConnectedHome.Discovery";

// discovery

const RESPONSE_DISCOVER = "DiscoverAppliancesResponse";
const REQUEST_DISCOVER = "DiscoverAppliancesRequest";

// control

const REQUEST_TURN_ON = "TurnOnRequest";
const RESPONSE_TURN_ON = "TurnOnConfirmation";

const REQUEST_TURN_OFF = "TurnOffRequest";
const RESPONSE_TURN_OFF = "TurnOffConfirmation";

const REQUEST_SET_PERCENTAGE = "SetPercentageRequest";
const RESPONSE_SET_PERCENTAGE = "SetPercentageConfirmation";

const REQUEST_INCREMENT_PERCENTAGE = "IncrementPercentageRequest";
const RESPONSE_INCREMENT_PERCENTAGE = "IncrementPercentageConfirmation";

const REQUEST_DECREMENT_PERCENTAGE = "DecrementPercentageRequest";
const RESPONSE_DECREMENT_PERCENTAGE = "DecrementPercentageConfirmation";

// errors

const ERROR_UNSUPPORTED_OPERATION = "UnsupportedOperationError";
const ERROR_UNEXPECTED_INFO = "UnexpectedInformationReceivedError";

/**
 * Main entry point.
 * Incoming events from Alexa Lighting APIs are processed via this method.
 */
exports.handler = function(event, context) {
  log('Received Directive', event);

  var requestedNamespace = event.header.namespace;
  switch (requestedNamespace) {

    /**
     * The namespace of "Discovery" indicates a request is being made to the lambda for
     * discovering all appliances associated with the customer's appliance cloud account.
     * can use the accessToken that is made available as part of the payload to determine
     * the customer.
     */
      case 'Alexa.ConnectedHome.Discovery':
          handleDiscovery(event, context);
          break;

    /**
     * The namespace of "Control" indicates a request is being made to us to turn a
     * given device on, off or brighten. This message comes with the "appliance"
     * parameter which indicates the appliance that needs to be acted on.
     */
      case 'Alexa.ConnectedHome.Control':
          handleControl(event, context);
          break;

    /**
     * We received an unexpected message
     */
      default:
          log('Err', 'No supported namespace: ' + event.header.namespace);
          context.fail('Something went wrong');
          break;
  }
};

/**
 * This method is invoked when we receive a "Discovery" message from Alexa Connected Home Skill.
 * We are expected to respond back with a list of appliances that we have discovered for a given
 * customer. 
 */
function handleDiscovery(event, context) {
  //  Creating response header.
  var headers = createHeader(NAMESPACE_DISCOVERY, RESPONSE_DISCOVER);
  var processedPayload = {};
  log('Discovery', 'Handling discovery..');
  if (event.header.name == REQUEST_DISCOVER) {
    requestForUserEmail(event)
      .then(requestForUserDeviceProfiles, log)
      .then(assembleApplianceIdObjects, log)
      .then(function packageAndSendPayload(applianceObjects) {
        processedPayload = {
          discoveredAppliances: applianceObjects
        };
        
        //  Craft the final response back to Alexa Connected Home Skill. 
        var result = {
          header: headers,
          payload: processedPayload
        };
  
        log('Discovery', result);
  
        context.succeed(result);
    }, log);
  }
}

/**
 * Control events are processed here.
 * This is called when Alexa requests an action (IE turn off appliance).
 */
function handleControl(event, context) {
  requestForDeviceServiceAccessToken(event)
    .then(constructDeviceCommand, log)
    .then(submitHttpRequest, log)
    .then(function notifyAlexa(result) {
        log('Control', result);
        context.succeed(result);
    }, function notifyAlexa(result) {
        log('Control', result);
        context.fail(result);
    });
}

function constructDeviceCommand(event) {
  var requestedName = event.header.name;
  var accessToken = event.payload.accessToken;
  
  
  var deviceTypeAndID = event.payload.appliance.applianceId;
  var deviceId = deviceTypeAndID.split("_").pop();
  var deviceType = deviceTypeAndID.split("_").shift();
  var message_id = event.header.messageId;
  
  var param = "";
  var state = 0;
  var confirmation;
  var funcName;
  
  if (deviceType === 'light') {
    deviceType = "0";
  } else if(deviceType === 'fan') {
    deviceType = "1";
  }
  
  log("Access Token: ", accessToken);
  log("DeviceID: ", deviceId);
  
  switch (requestedName) {
    case REQUEST_TURN_ON:
      confirmation = RESPONSE_TURN_ON;
      funcName = "on";
      param = deviceType;
      break;
  
    case REQUEST_TURN_OFF:
      confirmation = RESPONSE_TURN_OFF;
      funcName = "off";
      param = deviceType;
      break;
  
    case REQUEST_SET_PERCENTAGE:
      state = event.payload.percentageState.value;
      confirmation = RESPONSE_SET_PERCENTAGE;
      funcName = "setvalue";
      param = deviceType + "=" + state;
      break;
  
    case REQUEST_INCREMENT_PERCENTAGE:
      var increment = event.payload.deltaPercentage.value;
  
      state += increment;
  
      if (state > 100){
        state = 100;
      }
  
      incDecIdentifier = 1;
  
      confirmation = RESPONSE_INCREMENT_PERCENTAGE;
      funcName = "setvalue";
      param = deviceType + "=" + state + '=' + 1;
      break;
  
    case REQUEST_DECREMENT_PERCENTAGE:
      var decrement = event.payload.deltaPercentage.value;
  
      state = decrement;
  
      if(state < 0) {
        state = 0;
      }
  
      incDecIdentifier = 2;
  
      confirmation = RESPONSE_DECREMENT_PERCENTAGE;
      funcName = "setvalue";
      param = deviceType + "=" + state + '=' + 2;
      break;
      
     
    // 1 =  increment
    // 2 =  decrement
      
    default:
      console.log("nothing worked... :'(");
      break;
  }
  
  

  var serviceDeviceAccessAndCommands = "args=" + param;
  var devicePostPath = servicePostPath + deviceId + "/" + funcName;
  
  var postStruction = {
      auth: accessToken,
      path: devicePostPath,
      data: serviceDeviceAccessAndCommands,
      msg: confirmation
  };
  return postStruction;
}

function submitHttpRequest(postInfo) {
    
    // Submitting HTTPS REQUEST
  var options = {
    hostname: serviceHostName,
    port: 443,
    path: postInfo.path,
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Authorization': 'Bearer ' + postInfo.auth
    }
  };
  
  return new Promise(function promiseToSubmitHttpsRequest(resolve, reject) {
     var test = https.request(options, function httpsCallback(response) {
        
        response.on('error', function ohno(input) {
            
            var headers = createHeader(NAMESPACE_CONTROL, "TargetHardwareMalfunctionError");
            var payloads = {};
            var result = {
              header: headers,
              payload: payloads
            };
            var test = JSON.parse(input);
            console.log(input.toString('utf-8'));
            console.log(test.return_value);
            reject(result);
            
        });
        response.on('data', function ohyes(input) {
            var headers = createHeader(NAMESPACE_CONTROL, postInfo.msg);
            var payloads = {};
            var result = {
              header: headers,
              payload: payloads
            };
            var test = JSON.parse(input);
            console.log(input.toString('utf-8'));
            console.log(test.return_value);
            resolve(result);
        });
     });
      test.on('error', (e) => {
          console.log('Request Error: ' + e);
            
            var headers = createHeader(NAMESPACE_CONTROL, "TargetHardwareMalfunctionError");
            var payloads = {};
            var result = {
              header: headers,
              payload: payloads
            };
            var test = JSON.parse(e);
            console.log(e.toString('utf-8'));
            console.log(test.return_value);
            reject(result);
      });
      test.write(postInfo.data);
      test.end();
  });
}

/**
 * Utility functions.
 */
function log(title, msg) {
  console.log(title + ": " + msg);
}

function createMessageUUID() {
  var d = new Date().getTime();

  var uuid = 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
    var r = (d + Math.random()*16)%16 | 0;
    d = Math.floor(d/16);
    return (c=='x' ? r : (r&0x3|0x8)).toString(16);
  });

  return uuid;
}

function createHeader(namespace, name) {
  return {
   messageId : createMessageUUID(),
   namespace : namespace,
   name : name,
   payloadVersion : "2"
  };
}

function generateControlError(name, code, description) {
  var headers = createHeader(NAMESPACE_CONTROL, name);

  var payload = {
    exception: {
      code: code,
      description: description
    }
  };

  var result = {
    header: headers,
    payload: payload
  };

  return result;
}

function requestForUserEmail(event) {
  var accessToken = event.payload.accessToken;
  return new Promise(function promiseToRequestForUserEmail(resolve, reject) {
    var amazonProfileURL = 'https://api.amazon.com/user/profile?access_token=' + accessToken;
    
    console.log(amazonProfileURL);
    var getRequest = https.get(amazonProfileURL, function processProfileResponse(res) {
        res.on('data', function procInput(rawData) {
            var jsonData = JSON.parse(rawData);
            var userEmailAddr = jsonData.email;
            console.log(userEmailAddr);
            return resolve(userEmailAddr);
        });
    });
    getRequest.on('error', function ohno(err) {
        var processedData = JSON.parse(err);
        console.log("ERROR OUTSIDE" + processedData);
        reject(processedData);
    });
  });
}

function testRequestForUserEmail(accessToken) {
  return new Promise(function promiseToRequestForUserEmail(resolve, reject) {
    resolve("blah@amazon.com");
  });
}

function requestForUserDeviceProfiles(email) {
  var params = {
      'TableName': process.env.ALEXA_CREDS_TABLE,
      'Key': {'amzn_email': {'S': email}}
  };
  return retrieveItemFromDynamoDB(params);
}

function requestForDeviceServiceAccessToken(event) {
  var params = {
      'TableName': process.env.OAUTH_ACCESS_TOKEN_TABLE,
      'Key': {'service_id': {'S': process.env.SERVICE_ID }}
  };
  return retrieveItemFromDynamoDB(params)
          .then(function extractAccessToken(oauthObject) {
            event.payload.accessToken = oauthObject.access_token;
            return event;
          }, log);
}

function retrieveItemFromDynamoDB(getItemParam) {
  return new Promise(function promiseToRequestForUserDeviceProfiles(resolve, request) {
	  var dynamoDBData;

    db.getItem(getItemParam, function(err, data) {
      if (err) {
        console.log('Failed to get data from database', err);
        return reject("ERROR", "Cannot obtain device data from database.");
      } else {
        // Convert DynamoDB Wrapped JSON object into regular JSON object.
        dynamoDBData = attr.unwrap(data.Item);
        console.log(dynamoDBData);
        resolve(dynamoDBData);
      }
    });
  });
}

function createVenthoodLightObject(deviceID, friendlyDeviceName) {
  return {
    applianceId: "light_" + deviceID,
    manufacturerName: 'FirstBuild',
    modelName: 'Venthood',
    version: '0.0.3',
    friendlyName: friendlyDeviceName === "" ?  'Venthood Lights' : friendlyDeviceName,
    friendlyDescription: 'The lights in your FirstBuild made Voice Venthood.',
    isReachable: true,
    actions:[
      "incrementPercentage",
      "decrementPercentage",
      "turnOn",
      "turnOff"
    ]
  };
}

function createVenthoodFanObject(deviceID, friendlyDeviceName) {
  return {
    applianceId: "fan_" + deviceID,
    manufacturerName: 'FirstBuild',
    modelName: 'Venthood',
    version: '0.0.3',
    friendlyName: friendlyDeviceName === "" ?  'Venthood Exhaust' : friendlyDeviceName,
    friendlyDescription: 'The exhaust in your FirstBuild made Voice Venthood.',
    isReachable: true,
    actions:[
      "setPercentage",
      "turnOn",
      "turnOff"
    ]
  };
}

function assembleApplianceIdObjects(deviceIdPayload) {
  var appliances = [];
  var deviceID = deviceIdPayload.device_id;
  var fanName = deviceIdPayload.fan_device_name;
  var lightName = deviceIdPayload.light_device_name;
 
  appliances.push(createVenthoodFanObject(deviceID, fanName));
  appliances.push(createVenthoodLightObject(deviceID, lightName));
  console.log(appliances);
  
  return appliances;
}