<html>
<head>
<title>Log Sensor Data</title>
</head>
<body>
<?php
//Creating a web interface to access and modify our database

	//Variables to connect to the CSEE4240 database
	$sql_host = "localhost";
	$sql_user = "root";
	$sql_password ="";		
	$sql_dblink ="csee4240_proj";
	
	//Variables to use to store values into the database
	$mote_id = $_REQUEST['mote_id'];
	$radio_addr = $_REQUEST['radio_address'];
	$temperature = $_REQUEST['temperature'];
	$humidity = $_REQUEST['humidity'];
	$pressure = $_REQUEST['pressure'];
	$gps = $_REQUEST['gps'];
	date_default_timezone_set('America/New_York');
	$timestamp = date("Y-m-d H:i:s"); //Ex: 2013-03-13 17:16:18
		
	
	//Establishing a connection
	$db_conn = mysqli_connect($sql_host, $sql_user, $sql_password, $sql_dblink);
	if(mysqli_connect_errno()){
		die('Faliled to connect to MySQL: ' . mysqli_connect_error());
	}
	
	$sql = "INSERT INTO `csee4240_proj`.`logsensordata` (`mote_id`, `radio_address`, `temperature`, `humidity`, `pressure`, `gps`, `timestamp`) VALUES ($mote_id,$radio_addr,$temperature,$humidity,$pressure,$gps,'$timestamp')";
	//$sql= "INSERT INTO logsensordata(mote_id,radio_address, temperature, humidity, pressure, gps, timestamp) values ($mote_id,$radio_address,$temperature,$humidity,$pressure,'$gps','$timestamp')";
	echo $sql;
	$db_conn->query($sql);

	
?>
</body>
</html>