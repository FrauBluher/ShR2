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
</head>
<body id="page-top" data-spy="scroll" data-target=".navbar-fixed-top">
	<div id="navbar"></div>
	<!-- Page Content -->
	<section id="intro" class="intro-section">
		<div class="container">
			<div class="row">
				<div class="col-lg-12 text-center">
					<h1>It works!</h1>
					<p class="lead">Welcome to your new SEADS device.</p>
					<p>Before we get started, let's go over some setup steps.</p>
					<p>When you're ready, we'll gather some information about your network.</p> 
					<a class="btn btn-default page-scroll" href="#wifi">Get Started</a>
				</div>
			</div>
			<!-- /.row -->
		</div>
		<!-- /.container -->
	</section>

    <!-- About Section -->
    <section id="wifi" class="wifi-section">
        <div class="container">
            <div class="row">
                <div class="col-lg-12">
                    <h1>About Section</h1>
                    <a class="btn btn-default page-scroll" href="#services">Next</a>
                </div>
            </div>
        </div>
    </section>

    <!-- Services Section -->
    <section id="services" class="services-section">
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
    <section id="contact" class="contact-section">
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
	<!-- Bootstrap Core JavaScript -->
	<script src="/static/js/bootstrap.min.js"></script>
	<!-- Scrolling Nav JavaScript -->
    <script src="/static/js/jquery.easing.min.js"></script>
    <script src="/static/js/scrolling-nav.js"></script>
</body>
</html>
