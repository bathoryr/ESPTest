#pragma once
#include <Arduino.h>

const char log_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
<div>
    <div>
        Status log:<br/>
        <pre><div id="log"/></pre>
    </div>
  <button type="button" onclick="clnLog()">Clean log</button>
</div>

<script>
function clnLog() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/sendCmd?cmd=CLNLOG", true);
  xhttp.send();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("log").innerText = this.responseText;
    }
  };
  xhttp.open("GET", "/logText", true);
  xhttp.send();
}

setInterval(function() {
  getData();
}, 1000);
</script>
</body>
</html>
)=====";