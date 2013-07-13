<?php
// Establish socket
$sock = fsockopen("localhost", 55427, $errno, $errstr, 10);

if(!sock) { // error
	die("Socket didn't initialise!\n" . $errstr . "\nError code: " . $errno);
} else { // write JSON to the socket
	$out = json_encode(array("text" => "Hello world!", "badge" => 1337, "key" => "<insert device key here>"));
	fwrite($sock, $out . " d");
	fwrite($sock, "\n"); // this is newline-based protocol

	// wait for eof
	while(!feof($sock)) {
		echo fgets($sock, 128);
	}

	fclose($sock);
}

echo "\n";

?>
