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

var particleServer = "api.particle.io";
var particlePath = "/v1/devices/";

var venthoodManuLight = "70733a01-271c-491d-9f28-0182e887dd07";
var venthoodManuFan = "903459fd-c58d-4256-9150-b94c98492447";

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
const RESPONSE_INCREMENT_PERCENTAGE = "IncrementPercentageRequest";

const REQUEST_DECREMENT_PERCENTAGE = "DecrementPercentageRequest";
const RESPONSE_DECREMENT_PERCENTAGE = "DecrementPercentageRequest";

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
  var payload = {};
  
  if (event.header.name == REQUEST_DISCOVER) {
    requestForUserEmail(event.payload.accessToken)
      .then(requestForUserEmail, log)
      .then(assembleApplianceIdObjects, log)
      .then(function packageAndSendPayload(applianceObjects) {
        payload = {
          discoveredAppliances: applianceObjects
        };
  
        //  Craft the final response back to Alexa Connected Home Skill. 
        var result = {
          header: headers,
          payload: payloads
        };
  
        log('Discovery', result);
  
        context.succeed(result);
    });
  }
}

/**
 * Control events are processed here.
 * This is called when Alexa requests an action (IE turn off appliance).
 */
function handleControl(event, context) {
  if (event.header.namespace === 'Alexa.ConnectedHome.Control') {
    /**
     * Retrieve the appliance id and accessToken from the incoming message.
     */
    var requestedName = event.header.name;
    var accessToken = event.payload.accessToken;
    var applianceId = event.payload.appliance.applianceId;
    var deviceType = event.payload.appliance.additionalApplianceDetails.type;
    var message_id = event.header.messageId;
    var param = "";
    var state = 0;
    var index = "0";
    var confirmation;
    var funcName;
    var incDecIdentifier = "";

    log("Access Token: ", accessToken);
    log("DeviceID: ", deviceid);

    switch (requestedName) {
      case REQUEST_TURN_ON:
        state = 1;
        confirmation = "TurnOnConfirmation";
        funcName = "onoff";
        break;

      case REQUEST_TURN_OFF:
        state = 0;            
        confirmation = "TurnOffConfirmation";
        funcName = "onoff";
        break;

      case SET_PERCENTAGE_REQUEST:
        state = event.payload.percentageState.value;
        confirmation = "SetPercentageConfirmation";
        funcName = "setvalue";
        break;

      case INCREMENT_PERCENTAGE_REQUEST:
        var increment = event.payload.deltaPercentage.value;

        state += increment;

        if(state > 100){
          state = 100;
        }

        incDecIdentifier = 1;

        confirmation = "IncrementPercentageConfirmation";
        funcName = "setvalue";
        break;

      case DECREMENT_PERCENTAGE_REQUEST:
        var decrement = event.payload.deltaPercentage.value;

        state = decrement;

        if(state < 0){
          state = 0;
        }

        incDecIdentifier = 2;

        confirmation = "DecrementPercentageConfirmation";
        funcName = "setvalue";
        break;
    }

    if (deviceType === 'light') {
      index = "0";
    } else if(deviceType === 'fan') {
      index = "1";
    }

    if (incDecIdentifier === "") {
      param = index + "=" + state;
    } else {
      param = index + "=" + state + "=" + incDecIdentifier;
    }

    // Submitting HTTP REQEUST
    var options = {
      hostname: particleServer,
      port: 443,
      path: particlePath + deviceid + "/" + funcName,
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
      }
    };

    var data = "access_token=" + accessToken + "&" + "args=" + param;

    var serverError = function (e) {
      log('Error', e.message);
      context.fail(generateControlError(requestedName, 'DEPENDENT_SERVICE_UNAVAILABLE', 'Unable to connect to server'));
    };

    var callback = function(response) {
      var str = '';

      response.on('data', function(chunk) {
        str += chunk.toString('utf-8');
      });

      response.on('end', function() {
        log('Return Value');
        log(str);

        var headers = {
          namespace: 'Alexa.ConnectedHome.Control',
          name: confirmation,
          payloadVersion: '2',
          messageId: message_id
        };
        var payloads = {

        };
        var result = {
          header: headers,
          payload: payloads
        };

        context.succeed(result);
      });

      response.on('error', serverError);
    };

    var req = https.request(options, callback);

    req.on('error', serverError);

    req.write(data);
    req.end();
  }
}

function handleControlTurnOn(event) {
  var header = createHeader(NAMESPACE_CONTROL, RESPONSE_TURN_ON);
  var payload = {};
  return createDirective(header, payload);
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
  var headers = {
    namespace: NAMESPACE_CONTROL,
    name: name,
    payloadVersion: '2'
  };

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

function requestForUserEmail(accessToken) {
  return new Promise(function promiseToRequestForUserEmail(resolve, reject) {
    var amazonProfileURL = 'https://api.amazon.com/user/profile?access_token=' + accessToken;
    https.request(amazonProfileURL, function processProfileResponse(error, response, body) {
      if (response.statusCode == 200) {
        var profile = JSON.parse(body);
        resolve(profile.email);
      } else {
        reject("ERROR", "Cannot obtain user email (LWA).");
      }
    });
  });
}

function requestForUserDeviceProfiles(email) {
  return new Promise(function promiseToRequestForUserDeviceProfiles(resolve, request) {
	  var dynamoDBData;
    var params = {
        'TableName': process.env.ALEXA_CREDS_TABLE,
        'Key': {'user_email': {'S': email}}
    };

    db.getItem(params, function(err, data) {
      if (err) {
        console.log('Failed to get data from database', err);
        return reject("ERROR", "Cannot obtain device data from database.");
      } else {
        // Convert DynamoDB Wrapped JSON object into regular JSON object.
        console.log(data.Item);
        dynamoDBData = attr.unwrap(data.Item);
        resolve(dynamoDBData);
      }
    });
  });
}

function createVenthoodLightObject(deviceID, friendlyDeviceName) {
  return {
    applianceId: deviceID,
    manufacturerName: 'FirstBuild',
    modelName: 'Venthood',
    version: '0.0.3',
    friendlyName: friendlyDeviceName.isNaN() ?  'Venthood Lights' : friendlyDeviceName,
    friendlyDescription: 'The lights in your FirstBuild made Voice Venthood.',
    isReachable: true,
    actions:[
      "incrementPercentage",
      "decrementPercentage",
      "turnOn",
      "turnOff"
    ],
    additionalApplianceDetails: {
      type: "light"
    } 
  };
}

function createVenthoodFanObject(deviceID, friendlyDeviceName) {
  return {
    applianceId: deviceID,
    manufacturerName: 'FirstBuild',
    modelName: 'Venthood',
    version: '0.0.3',
    friendlyName: friendlyDeviceName.isNaN() ?  'Venthood Exhaust' : friendlyDeviceName,
    friendlyDescription: 'The exhaust in your FirstBuild made Voice Venthood.',
    isReachable: true,
    actions:[
      "setPercentage",
      "turnOn",
      "turnOff"
    ],
    additionalApplianceDetails: {
      type: "fan"
    }
  };
}

function assembleApplianceIdObjects(deviceIdPayload) {
  var appliances = {};
  var deviceID = deviceIdPayload.device-id;
  var fanName = deviceIdPayload.fan-device-name;
  var lightName = deviceIdPayload.light-device-name;
  
  appliances.push(createVenthoodFanObject(deviceID, fanName));
  appliances.push(createVenthoodLightObject(deviceID, light));
  return appliances;
}
