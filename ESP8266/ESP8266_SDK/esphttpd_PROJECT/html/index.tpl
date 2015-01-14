<html>
<head>
<title>ESP8266 - esphttpd</title>
<!-- Bootstrap Core CSS -->
<link rel="stylesheet" type="text/css" href="/static/css/bootstrap.min.css">
<!-- Custom CSS -->
<link href="/static/css/scrolling-nav.css" rel="stylesheet">
<!-- jQuery Version 1.11.1 -->
<script src="/static/js/jquery.min.js"></script>
<script>
$(function(){
	$("#navbar").load("/navbar.html");
});
</script>

<script type="text/javascript" src="/static/js/140medley.min.js"></script>
<!-- jQuery Version 1.11.1 -->
<script type="text/javascript">

var xhr=j();
var currAp="%currSsid%";

function createInputForAp(ap) {
    if (ap.essid=="" && ap.rssi==0) return;
    var table = $("#aps");
    var row = table.insertRow();
    var rssi=document.createElement("div");
    var rssiVal=-Math.floor(ap.rssi/51)*32;
    rssi.className="icon";
    rssi.style.backgroundPosition="0px "+rssiVal+"px";
    var encrypt=document.createElement("div");
    if (ap.enc > 0) {
        span = document.createElement("span");
        span.className="glyphicon glyphicon-signal";
        encrypt.appendChild(span);
    }
    //var encVal="-64"; //assume wpa/wpa2
    //if (ap.enc=="0") encVal="0"; //open
    //if (ap.enc=="1") encVal="-32"; //wep
    //encrypt.className="icon";
    //encrypt.style.backgroundPosition="-32px "+encVal+"px";
    var input=document.createElement("input");
    input.type="radio";
    input.name="essid";
    input.value=ap.essid;
    if (currAp==ap.essid) input.checked="1";
    input.id="opt-"+ap.essid;
    var label=document.createElement("label");
    label.htmlFor="opt-"+ap.essid;
    label.textContent=ap.essid;
    row.insertCell(input);
    row.insertCell(rssi);
    row.insertCell(encrypt);
    row.insertCell(label);
}

function getSelectedEssid() {
    var e=document.forms.wifiform.elements;
    for (var i=0; i<e.length; i++) {
        if (e[i].type=="radio" && e[i].checked) return e[i].value;
    }
    return currAp;
}


function scanAPs() {
    xhr.open("GET", "/wifi/wifiscan.cgi");
    xhr.onreadystatechange=function() {
        if (xhr.readyState==4 && xhr.status>=200 && xhr.status<300) {
            var data=JSON.parse(xhr.responseText);
            currAp=getSelectedEssid();
            if (data.result.inProgress=="0" && data.result.APs.length>1) {
                // Create table to hold APs
                $("#aps").empty()
                         .removeClass()
                         .addClass("table-responsive")
                var table = document.createElement("table");
                table.className="table";
                for (var i=0; i<data.result.APs.length; i++) {
                    if (data.result.APs[i].essid=="" && data.result.APs[i].rssi==0) continue;
                    $("#aps").append(createInputForAp(data.result.APs[i]));
                }
                window.setTimeout(scanAPs, 20000);
            } else {
                window.setTimeout(scanAPs, 1000);
            }
        }
    }
    xhr.send();
}


window.onload=function(e) {
    scanAPs();
};
</script>

</head>
<body id="page-top" data-spy="scroll" data-target=".navbar-fixed-top">
	<div id="navbar"></div>
	<!-- Page Content -->
	<section id="intro" class="top-section light">
		<div class="container">
			<div class="row">
				<div class="col-lg-12 text-center">
					<h1>It works!</h1>
					<p class="lead">Welcome to your new SEADS device.</p>
					<p>Before we get started, let's go over some setup steps.</p>
					<p>When you're ready, we'll gather some information about your network.</p> 
					<a class="btn btn-default page-scroll" href="#wifi">Begin Setup</a>
				</div>
			</div>
			<!-- /.row -->
		</div>
		<!-- /.container -->
	</section>

    <!-- About Section -->
    <section id="wifi" class="scroll-section dark">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 text-centered">
                    <h1>WiFi Scan</h1>
                    <p>Current WiFi mode: %WiFiMode%</p>
                    <p>Note: %WiFiapwarn%</p>
                    <form name="wifiform" action="/wifi/connect.cgi" method="post">
                    <p>
                        To connect to a WiFi network, please select one of the detected networks...<br>
                        <div id="aps">Scanning...</div>
                        <br>WiFi password, if applicable: <br />
                        <input type="text" name="passwd" val="%WiFiPasswd%"> <br />
                        <input type="submit" name="connect" value="Connect!">
                    </p>
                    <a class="btn btn-default page-scroll" href="#services">Next</a>
                </div>
            </div>
        </div>
    </section>

    <!-- Services Section -->
    <section id="services" class="scroll-section light">
        <div class="container">
            <div class="row">
                <div class="col-lg-12">
                    <h1>Services Section</h1>
                    <p>Under Construction<p>
                    <a class="btn btn-default page-scroll" href="#contact">Next</a>
                </div>
            </div>
        </div>
    </section>

    <!-- Contact Section -->
    <section id="contact" class="scroll-section dark">
        <div class="container">
            <div class="row">
                <div class="col-lg-12">
                    <h1>Contact Section</h1>
                    <p>Fyi, this page has been loaded <b>%counter%</b> times.</p>
                    <ul class="list-unstyled">
					<li>Go to <a href="/wifi">WiFi Page</a>.</li>
					<li>You can also control the <a href="led.tpl">LED</a>.</li>
					<li>You can download the raw <a href="flash.bin">contents</a> of the SPI flash rom</li>
					</ul>
					<a class="btn btn-default" href="#">Done!</a>
                </div>
            </div>
        </div>
    </section>
</body>

<script>
// Bootstrap Core JavaScript
$.getScript("/static/js/bootstrap.min.js");
$.getScript("/static/js/jquery.easing.min.js");
$.getScript("/static/js/scrolling-nav.js");
</script>

</html>
