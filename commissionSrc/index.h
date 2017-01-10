const char index_html[] =
"<!DOCTYPE html>\
<html>\
    <head>\
        <meta name='viewport' content='width=device-width, initial-scale=1'>\
        <title>Setup your Venthood</title>\
        <link rel='stylesheet' type='text/css' href='style.css'>\
    </head>\
    <body>\
        <h1>FirstBuBuBuild!!!!</h1>\
        <h2>Connect me to your WiFi!</h2>\
        <div id='scan-div'>\
            <h3>Scan for visible WiFi networks</h3>\
            <button id='scan-button' type='button'>Scan</button>\
        </div>\
        <div id='networks-div'></div>\
        <div id='connect-div' style='display: none'>\
            <p>Don't see your network? Move me closer to your router, then re-scan.</p>\
            <form id='connect-form'><input type='password' id='password' size='25' placeholder='password' />\
              <button type='button' class='input-helper' id='show-button'>Show</button>\
              <button type='submit' id='connect-button'>Connect</button>\
            </form>\
        </div>\
        <div id='device-div' style='display: block'>\
            <p>Add your device names for the Venthood Light and Fan.</p>\
            <form id='light-form'><input type='text' id='light' size='25' placeholder='Light' /></form>\
            <form id='fan-form'><input type='text' id='fan' size='25' placeholder='Fan' /></form>\
            <form id='amazon-email-form'><input type='text' id='email' size='25' placeholder='Amazon Email' /></form>\
            <button type='submit' id='update-button'>Connect</button>\
        </div>\
        <script src='rsa-utils/jsbn_1.js'></script>\
        <script src='rsa-utils/jsbn_2.js'></script>\
        <script src='rsa-utils/prng4.js'></script>\
        <script src='rsa-utils/rng.js'></script>\
        <script src='rsa-utils/rsa.js'></script>\
        <script src='script.js'></script>\
    </body>\
</html>\
";
