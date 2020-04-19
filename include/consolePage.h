#pragma once
#include <Arduino.h>

const char console_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
<div>
    <div>
        Console:<br/>
        <pre><div id="console"/></pre>
    </div>
    <input type="text" id="cmd">
	<button type="button" onclick="sendData()">Send command</button> 
  <button type="button" onclick="clrScr()">Clear screen</button>
  <div>
    <button type="button" onclick="doReset()">RESET</button>
  </div>
</div>

<script>
function doReset() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/sendCmd?cmd=RESET", true);
  xhttp.send();
}

function clrScr() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/sendCmd?cmd=CLRSCR", true);
  xhttp.send();
}

function sendData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("console").innerText = document.getElementById("console").innerText +
        this.responseText;
    }
  };  
  xhttp.open("GET", "/sendCmd?cmd=" + document.getElementById("cmd").value, true);
  xhttp.send();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("console").innerText = this.responseText;
    }
  };
  xhttp.open("GET", "/consoleText", true);
  xhttp.send();
}

setInterval(function() {
  getData();
}, 2000);
</script>
</body>
</html>
)=====";