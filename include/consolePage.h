#include <Arduino.h>

const char console_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<div id="demo">
    <div>
        Console:<br/>
        <pre><div id="console"/></pre>
        Console Value is : <span id="conStat">0</span><br>
        Command State is : <span id="cmdStat">NA</span>
    </div>
    <input type="text" id="cmd">
	<button type="button" onclick="sendData()">Send command</button>
</div>

<script>
function sendData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("console").innerText = document.getElementById("console").innerText +
        this.responseText;
      document.getElementById("cmdStat").innerHTML = this.responseURL;
    }
  };  
  xhttp.open("GET", "/sendCmd?cmd=" + document.getElementById("cmd").value, true);
  xhttp.send();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("console").innerText =
      this.responseText;
      document.getElementById("conStat").innerHTML = this.responseURL;
    }
  };
  xhttp.open("GET", "/consoleText", true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //2000mSeconds update rate

/*
document.addEventListener('DOMContentLoaded', function() {
    getData();
}, false);
*/
</script>
</body>
</html>
)=====";