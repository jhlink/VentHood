const char script_js[] =
"var base_url = 'http://192.168.0.1/';\
var network_list;\
var public_key;\
var rsa = new RSAKey();\
var scanButton = document.getElementById('scan-button');\
var connectButton = document.getElementById('connect-button');\
var copyButton = document.getElementById('copy-button');\
var showButton = document.getElementById('show-button');\
var deviceID = document.getElementById('device-id');\



var copy = function () {\
    window.prompt('Copy to clipboard: Ctrl + C, Enter', deviceID.value);\
};\
var toggleShow = function () {\
    var a = document.getElementById('password');\
    inputType = a.type;\
    if (inputType === 'password') {\
        showButton.innerHTML = 'Hide';\
        a.type = 'text';\
    } else {\
        showButton.innerHTML = 'Show';\
        a.type = 'password';\
    }\
};\
var getRequest = function (a, b) {\
    var c = new XMLHttpRequest();\
    c.open('GET', a, true);\
    c.timeout = 8000;\
    c.send();\
    c.onreadystatechange = function () {\
        if (c.readyState == 4)\
            if (b) {\
                if (c.status == 200) {\
                    if (b.success) b.success(JSON.parse(c.responseText));\
                } else if (b.error) b.error(c.status, c.responseText);\
                if (b.regardless) b.regardless();\
            }\
    };\
};\
var postRequest = function (a, b, c) {\
    var d = JSON.stringify(b);\
    var e = new XMLHttpRequest();\
    e.open('POST', a, true);\
    e.timeout = 4000;\
    e.setRequestHeader('Content-Type', 'multipart/form-data');\
    e.send(d);\
    e.onreadystatechange = function () {\
        if (e.readyState == 4)\
            if (c) {\
                if (e.status == 200) {\
                    if (c.success) c.success(JSON.parse(e.responseText));\
                } else if (c.error) c.error(e.status, e.responseText);\
                if (c.regardless) c.regardless();\
            }\
    };\
};\
if (scanButton.addEventListener) {\
    copyButton.addEventListener('click', copy);\
    showButton.addEventListener('click', toggleShow);\
    scanButton.addEventListener('click', scan);\
    connectForm.addEventListener('submit', configure);\
} else if (scanButton.attachEvent) {\
    copyButton.attachEvent('onclick', copy);\
    showButton.attachEvent('onclick', toggleShow);\
    scanButton.attachEvent('onclick', scan);\
    connectForm.attachEvent('onsubmit', configure);\
}\
getRequest(base_url + 'device-id', device_id_callback);\
getRequest(base_url + 'public-key', public_key_callback);\
";
