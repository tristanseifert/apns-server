<?php
// Establish socket
$sock = fsockopen("localhost", 55427, $errno, $errstr, 10);

$testDevToken = "f22c59d10416f020286c8db122783186cd1d3f3e8a38b251bbb9c245ba6754cb";

if(!$sock) { // error
	die("Socket didn't initialise!\n" . $errstr . "\nError code: " . $errno . "\n");
} else { // write JSON to the socket
	$out = json_encode(array("text" => "Hello world!", "badge" => 1337, "key" => $testDevToken));
	fwrite($sock, $out);
	fwrite($sock, "\n"); // this is newline-based protocol

	// wait for eof
	while(!feof($sock)) {
		echo fgets($sock, 128);
	}

	fclose($sock);
}

echo "\n";

?>
