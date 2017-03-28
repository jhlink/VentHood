var scanButton=document.getElementById('scan-button');
var venthoodForm = document.getElementById('venthood-form');
var updateButton = document.getElementById('update-button');
function togObjDisp(childID, dispSet) {
  var elm = document.getElementById(childID);
  elm.style.display = dispSet;
}
var device_configure = function(a) {
  a.preventDefault();
  var light = document.getElementById('light').value;
  var fan = document.getElementById('fan').value;
  var email = document.getElementById('email').value;
  if (!email) {
      return false;
  }
  var d = {
    idx: 0,
    lightDeviceName: light,
    fanDeviceName: fan,
    amznEmail: email
  };
  updateButton.innerHTML = 'Sending configuration...';
  disableUpdateBtn();
  console.log('Sending credentials: ' + JSON.stringify(d));
  postRequest(base_url + 'cfg', d, device_configure_callback);
};
var device_configure_callback = {
  success: function(a) {
    console.log('Configuration received.');
    updateButton.innerHTML = 'Configuration received...';
    togObjDisp('logo','none');
    togObjDisp('hood-device-div','none');
    togObjDisp('scan-div','block');
    document.getElementById('stage2').className = 'circle';
  },
  error: function(a, b) {
    console.log('Configuration error: ' + a);
    updateButton.innerHTML = 'Retry';
    enableUpdateBtn();
  }
};
var disableUpdateBtn = function() {
  updateButton.disabled = true;
};
var enableUpdateBtn = function() {
  updateButton.disabled = false;
};
if (updateButton.addEventListener) {
  venthoodForm.addEventListener('submit', device_configure);
} else if (updateButton.attachEvent) {
  venthoodForm.attachEvent('onsubmit', device_configure);
}
var interval = setInterval(function() {
  if (document.readyState === 'complete') {
    clearInterval(interval);
    window.alert('You dont have to change the names but it can be easier if you get more connected lights or fans later on. Good examples of names to use are clear and easy for Alexa, like exhaust, lamp, turbo');
  }
}, 50);
