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

'use strict';

var credentials = {
  client: {
    id: process.env.CLIENT_ID,
    secret: process.env.CLIENT_SECRET
  },
  auth: {
    tokenHost: process.env.TOKEN_HOST,
  }
};

var oauth2 = require('simple-oauth2').create(credentials);
var attr = require('dynamodb-data-types').AttributeValue;
var aws = require('aws-sdk');

var db = new aws.DynamoDB();

function getOauthTokenFromService(targetServiceID) {
  var tokenConfig = { 'expires_in' : process.env.EXP_TIME_IN_SEC};
  return new Promise(function promiseToGetNewTokenFromService(resolve, reject) {
    oauth2.clientCredentials.getToken(tokenConfig, function(error, result) {
      if (error) {
        console.log('Cannot get Access Token from Service Provider:', error.message);
        return reject(null);
      }
    
      //  This returns the token object from the AccessToken class.
      var token = oauth2.accessToken.create(result).token;
      var expireAtDate = Date.parse(token.expires_at).toString();

      // Payload in JSON
      var payload = token;
      payload.service_id = targetServiceID;
      payload.expires_at = expireAtDate;
    
      console.log('Successfully requested token from service');
      console.log(payload);
      resolve(payload);
    });
  });
}

function refreshOauthTokenFromService(inputToken) {
  var serviceID = inputToken.service_id;
  delete inputToken.service_id;
  inputToken.expires_in = process.env.EXP_TIME_IN_SEC;

  return new Promise(function refreshOauthTokenFromService(resolve, reject) {
    var tokenObject = oauth2.accessToken.create(inputToken);

    // Note: Refresh function in simple-oauth2 library was modified to
    //  use inputToken.expires_in to request for a configuration
    //  dependent expiration tiem.
    tokenObject.refresh(function(error, result) {

      if (error) {
        return reject('Error in refreshing oauth token.', error);
      }

      var token = result.token; 
      token.expires_at = Date.parse(token.expires_at).toString();
      token.service_id = serviceID;

      console.log('Token refresh successful.', token);
      resolve(token);
    });
  });
}

function storeDataIntoDatabase(tokenPayloadInJson) {
	//	Convert JSON object into DynamoDB Wrapped JSON object.
	var dynamoDBData = attr.wrap(tokenPayloadInJson);

  // Create DynamoDB JSON
  var params = {
      TableName:process.env.OAUTH_ACCESS_TOKEN_TABLE, 
      Item: dynamoDBData
  };

  db.putItem(params,function(err,data) {
      if (err) {
        console.log('Failed to put data\n', err);
        return "Failure with putItem into database.";
      }
      console.log('Data put in DB\n', params);
      return "Success with putItem into database.";
  });
}

function getDataFromDatabase(targetServiceID) {
  return new Promise(function promiseToGetDataFromDB(resolve, reject) {
	  var dynamoDBData;
    var params = {
        'TableName': process.env.OAUTH_ACCESS_TOKEN_TABLE,
        'Key': {'service_id': {'S': targetServiceID}}
    };

    db.getItem(params, function(err, data) {
      if (err) {
        console.log('Failed to get data from database\n', err);
        return reject(targetServiceID);
      } else {
        // Convert DynamoDB Wrapped JSON object into regular JSON object.
        console.log(data.Item);
        dynamoDBData = attr.unwrap(data.Item);
        resolve(dynamoDBData);
      }
    });
  });
}

function renewOauthToken(targetServiceID) {
  getDataFromDatabase(targetServiceID).then(
    refreshOauthTokenFromService(inputToken),
    getOauthTokenFromService(targetServiceID))
  .then(
    storeDataIntoDatabase(inputToken),
    getOauthTokenFromService(targetServiceID))
  .then(
    storeDataIntoDatabase(inputToken));
}

exports.handler = function(event, context) {
  console.log('Refreshing token');

  switch(event.stage){
    case 1:
      getOauthTokenFromService(process.env.SERVICE_ID);
      break;
    case 2:
      getDataFromDatabase(process.env.SERVICE_ID)
        .then(refreshOauthTokenFromService)
        .then(storeDataIntoDatabase);
      break;
    case 3:
      getDataFromDatabase(process.env.SERVICE_ID);
      break;
    case 4:
      getDataFromDatabase(process.env.SERVICE_ID)
        .then(refreshOauthTokenFromService);
      break;
    case 5:
      getDataFromDatabase(process.env.SERVICE_ID)
        .then(getOauthTokenFromService);
      break;
    case 6:
      refreshOauthTokenFromService(process.env.SERVICE_ID);
      break;
    case 7:
      getOauthTokenFromService(process.env.SERVICE_ID)
        .then(storeDataIntoDatabase);
      break;
  }
};
