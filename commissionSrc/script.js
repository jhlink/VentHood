var base_url = 'http://192.168.0.1/';
var network_list;
var public_key;
var rsa = new RSAKey();
var scanButton = document.getElementById('scan-button');
var connectButton = document.getElementById('connect-button');
var copyButton = document.getElementById('copy-button');
var showButton = document.getElementById('show-button');
var deviceID = document.getElementById('device-id');
var connectForm = document.getElementById('connect-form');
var venthoodForm = document.getElementById('venthood-form');
var updateButton = document.getElementById('update-button');

var public_key_callback = {
    success: function(a) {
        console.log('Public key: ' + a.b);
        public_key = a.b;
        rsa.setPublic(public_key.substring(58, 58 + 256), public_key.substring(318, 318 + 6));
    },
    error: function(a, b) {
        console.log(a);
        window.alert('There was a problem fetching important information from your device. Please verify your connection, then reload this page.');
    }
};
var device_id_callback = {
    success: function(a) {
        var b = a.id;
        deviceID.value = b;
    },
    error: function(a, b) {
        console.log(a);
        var c = 'COMMUNICATION_ERROR';
        deviceID.value = c;
    }
};
var scan = function() {
  console.log('Scanning...!');
  disableButtons();
  scanButton.innerHTML = 'Scanning...';
  connectButton.innerHTML = 'Connect';
  document.getElementById('connect-div').style.display = 'none';
  document.getElementById('networks-div').style.display = 'none';
  getRequest(base_url + 'scan-ap', scan_callback);
};
var scan_callback = {
    success: function(a) {
        network_list = a.scans;
        console.log('I found:');
        var b = document.getElementById('networks-div');
        b.innerHTML = '';
        if (network_list.length > 0)
            for (var c = 0; c < network_list.length; c++) {
                ssid = network_list[c].ssid;
                console.log(network_list[c]);
                add_wifi_option(b, ssid);
                document.getElementById('connect-div').style.display = 'block';
            } else b.innerHTML = '<p> No networks found.</p>';
    },
    error: function(a) {
        console.log('Scanning error:' + a);
        document.getElementById('networks-div').innerHTML = '<p> Scanning error.</p>';
    },
    regardless: function() {
        scanButton.innerHTML = 'Re-Scan';
        enableButtons();
        document.getElementById('networks-div').style.display = 'block';
    }
};
var configure = function(a) {
    a.preventDefault();
    var b = get_selected_network();
    var c = document.getElementById('password').value;
    if (!b) {
        window.alert('Please select a network!');
        return false;
    }
    var d = {
        idx: 0,
        ssid: b.ssid,
        pwd: rsa.encrypt(c),
        sec: b.sec,
        ch: b.ch
    };
    connectButton.innerHTML = 'Sending credentials...';
    disableButtons();
    console.log('Sending credentials: ' + JSON.stringify(d));
    postRequest(base_url + 'configure-ap', d, configure_callback);
};
var configure_callback = {
    success: function(a) {
        console.log('Credentials received.');
        connectButton.innerHTML = 'Credentials received...';
        window.alert('Establishing connection... beep beep...');
        postRequest(base_url + 'connect-ap', {
            idx: 0
        }, connect_callback);
    },
    error: function(a, b) {
        console.log('Configure error: ' + a);
        window.alert('Got probs');
        connectButton.innerHTML='Retry';
        enableButtons();
    }
};
var connect_callback = {
    success: function(a) {
        console.log('Attempting to connect to the cloud.');
        connectButton.innerHTML = 'Attempting to connect...';
    },
    error: function(a, b) {
        console.log('Connect error:' + a);
        window.alert('The connect command failed, verification connection to Photon and retry.');
        connectButton.innerHTML = 'Retry';
        enableButtons();
    }
};
var device_configure = function(a) {
    a.preventDefault();
    var light = document.getElementById('light').value;
    var fan = document.getElementById('fan').value;
    var email = document.getElementById('email').value;
    var d = {
        idx: 0,
        lightNm: light,
        fanNm: fan,
        email: email
    };
    updateButton.innerHTML = 'Sending configuration...';
    disableUpdate();
    console.log('Sending credentials: ' + JSON.stringify(d));
    postRequest(base_url + 'cfg', d, configure_callback);
};
var device_configure_callback = {
    success: function(a) {
        console.log('Configuration received.');
        updateButton.innerHTML = 'Configuration received...';
        window.alert('Commencing device naming... beep beep...');
    },
    error: function(a, b) {
        console.log('Configuration error: ' + a);
        window.alert('We got probs. Find Jim for help.');
        updateButton.innerHTML='Retry';
        enableUpdate();
    }
};
var disableUpdate = function() {
    updateButton.disabled = true;
};
var enableUpdate = function() {
    updateButton.disabled = false;
};
var disableButtons = function() {
    connectButton.disabled = true;
    scanButton.disabled = true;
};
var enableButtons = function() {
    connectButton.disabled = false;
    scanButton.disabled = false;
};
var add_wifi_option = function(a, b) {
    var c = document.createElement('INPUT');
    c.type = 'radio';
    c.value = b;
    c.id = b;
    c.name = 'ssid';
    c.className = 'radio';
    var d = document.createElement('DIV');
    d.className = 'radio-div';
    d.appendChild(c);
    var e = document.createElement('label');
    e.htmlFor = b;
    e.innerHTML = b;
    d.appendChild(e);
    a.appendChild(d);
};
var get_selected_network = function() {
    for (var a = 0; a < network_list.length; a++) {
        ssid = network_list[a].ssid;
        if (document.getElementById(ssid).checked) return network_list[a];
    }
};
var copy = function() {
    window.prompt('Copy to clipboard: Ctrl + C, Enter', deviceID.value);
};
var toggleShow = function() {
    var a = document.getElementById('password');
    inputType = a.type;
    if (inputType === 'password') {
        showButton.innerHTML = 'Hide';
        a.type = 'text';
    } else {
        showButton.innerHTML = 'Show';
        a.type = 'password';
    }
};
var getRequest = function(a, b) {
    var c = new XMLHttpRequest();
    c.open('GET', a, true);
    c.timeout = 8000;
    c.send();
    c.onreadystatechange = function() {
        if (c.readyState == 4)
            if (b) {
                if (c.status == 200) {
                    if (b.success) b.success(JSON.parse(c.responseText));
                } else if (b.error) b.error(c.status, c.responseText);
                if (b.regardless) b.regardless();
            }
    };
};
var postRequest = function(a, b, c) {
    var d = JSON.stringify(b);
    var e = new XMLHttpRequest();
    e.open('POST', a, true);
    e.timeout = 4000;
    e.setRequestHeader('Content-Type', 'multipart/form-data');
    e.send(d);
    e.onreadystatechange = function() {
        if (e.readyState == 4)
            if (c) {
                if (e.status == 200) {
                    if (c.success) c.success(JSON.parse(e.responseText));
                } else if (c.error) c.error(e.status, e.responseText);
                if (c.regardless) c.regardless();
            }
    };
};
if (scanButton.addEventListener) {
    copyButton.addEventListener('click', copy);
    showButton.addEventListener('click', toggleShow);
    scanButton.addEventListener('click', scan);
    connectForm.addEventListener('submit', configure);
    venthoodForm.addEventListener('submit', device_configure);
} else if (scanButton.attachEvent) {
    copyButton.attachEvent('onclick', copy);
    showButton.attachEvent('onclick', toggleShow);
    scanButton.attachEvent('onclick', scan);
    connectForm.attachEvent('onsubmit', configure);
    venthoodForm.attachEvent('onsubmit', device_configure);
}
getRequest(base_url + 'device-id', device_id_callback);
getRequest(base_url + 'public-key', public_key_callback);
