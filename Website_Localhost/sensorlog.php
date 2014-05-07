<!DOCTYPE HTML>
<html>

<head>
  <title>Antarctica Sensoring Smartly</title>
  <meta name="description" content="website description" />
  <meta name="keywords" content="website keywords, website keywords" />
  <meta http-equiv="content-type" content="text/html; charset=windows-1252" />
  <link rel="stylesheet" type="text/css" href="style/style.css" />
</head>

<body>
  <div id="main">
    <div id="header">
      <div id="logo">
        <div id="logo_text">
          <!-- class="logo_colour", allows you to change the colour of the text -->
          <h1><a href="home.html">Antarctica <span class="logo_colour"><b>Sensoring</b></span><span style="color:red;font-weight:bold"> Smartly </span></a></h1>
          <h2>CSEE4240 Final Project 2014. Website <br>
				Eric Refour	</h2>
        </div>
      </div>
      <div id="menubar">
        <ul id="menu">
          <!-- put class="selected" in the li tag for the selected page - to highlight which page you're on -->
          <li class="selected"><a href="home.html"> Home</a></li>
          <li><a href="examples.html">Examples</a></li>
          <li><a href="sensorlog.php">Sensor Log</a></li>
          <li><a href="sensormote_error_report.php">Sensor-Mote Error Report</a></li>
          <li><a href="contact.html">Contact Us</a></li>
        </ul>
      </div>
    </div>
    <div id="site_content_wider">
      <div class="sidebar">
      <!-- NOTE: If the sidebar is going to be used, CHANGE DIV ID to site_content and content INSTEAD of site_content_wider and content_wider
	   <h1>Latest News</h1>
        <h4>New Website Launched</h4>
        <h5>January 1st, 2010</h5>
        <p>2010 sees the redesign of our website. Take a look around and let us know what you think.<br /><a href="#">Read more</a></p>
        <h1>Useful Links</h1>
        <ul>
          <li><a href="#">link 1</a></li>
          <li><a href="#">link 2</a></li>
          <li><a href="#">link 3</a></li>
          <li><a href="#">link 4</a></li>
        </ul>
        <h1>Search</h1>
        <form method="post" action="#" id="search_form">
          <p>
            <input class="search" type="text" name="search_field" value="Enter keywords....." />
            <input name="search" type="image" style="border: 0; margin: 0 0 -9px 5px;" src="style/search.png" alt="Search" title="Search" />
          </p>
        </form>
		-->
      </div>
	  
      <div id="content_wider">
        <h1>Sensor Log Page</h1>
        <p>Antarctica Sensoring Smartly provides users with the opportunity to view the data being collected from the robotic sensoring systems through its <strong>Sensor Log feature</strong>.
		   The robotic sensoring systems provide the data readings for their corresponding sensors along with a timestamp of when the data was collected. </p>
    
		<h2>Table</h2>
        <p>The Sensor Log Table is shown below, displaying the data for: Temperature (in degrees Celsius), Humidity (%rh), Pressure, and GPS Location.</p>
		<table style="width:100%; border-spacing:0;">
			<tr> <th>Entry #</th><th>Mote ID</th> <th>Radio Address</th><th>Temperature(Celsius)</th><th>Humidity(%rh)</th><th>Pressure(hPa)</th><th>GPS Location</th><th>Timestamp</th></tr>
			
			<?php
				//Variables to connect to the CSEE4240 database
				$sql_host = "localhost";
				$sql_user = "root";
				$sql_password ="";		
				$sql_dblink ="csee4240_proj";
				
				//Establishing a connection
				$db_conn = mysqli_connect($sql_host, $sql_user, $sql_password, $sql_dblink);
				if(mysqli_connect_errno()){
					die('Faliled to connect to MySQL: ' . mysqli_connect_error());
				}
				
				//Writing query to select rows from the 'LOGSENSORDATA TABLE' in the. NOTE: Select up to 20 entries at a time. Have to click on "See More Entries" link to see 20 more
				$select_sensordata_query = "SELECT * FROM `csee4240_proj`.`logsensordata`";
				$result_selectQuery = $db_conn->query($select_sensordata_query);
				if(!$result_selectQuery){
					die('Error with SELECT SensorLogData Query! Website Code: SensorLog Page');
				}
				while($row_SLDArray = $result_selectQuery->fetch_array()){
			?>
			<tr> <!--Row Beginning-->
				<td><?php 	echo $row_SLDArray['entry'] 	?></td>
				<td><?php 	echo $row_SLDArray['mote_id'] 	?></td>
				<td><?php 	echo $row_SLDArray['radio_address'] 	?></td>
				<td><?php 	echo $row_SLDArray['temperature'] 	?></td>
				<td><?php 	echo $row_SLDArray['humidity'] 	?></td>
				<td><?php 	echo $row_SLDArray['pressure'] 	?></td>
				<td><?php 	echo $row_SLDArray['gps'] 	?></td>
				<td><?php 	echo $row_SLDArray['timestamp'] 	?></td>
			</tr> <!--Row ending-->
			<!--<tr><td>Item 2</td><td>Description of Item 2</td></tr> -->
			<?php
				}//While loop ending bracket
			?>
		</table>
		
	</div>
    </div>
     <div id="footer">
      <p><a href="home.html">Home</a> | <a href="examples.html">Examples</a> | <a href="sensorlog.php">Sensor Log</a> | <a href="sensormote_error_report.php">Sensor-Mote Error Report</a> | <a href="contact.html">Contact Us</a></p>
      <p><span style="color:gray">Copyright &copy; night_sky</span> | <a href="http://validator.w3.org/check?uri=referer">HTML5</a> | <a href="http://jigsaw.w3.org/css-validator/check/referer">CSS</a> | <a href="http://www.html5webtemplates.co.uk">design from HTML5webtemplates.co.uk</a> | <a><span style="color:gray"> Modified & Created by Eric Refour</span></a></p>
    </div>
  </div>
</body>
</html>
