<?php

// -----------------------------------------------------------------------------
// PHP string functions: 
// https://www.w3schools.com/php/php_ref_string.asp

function startsWith($string, $startString) {
    $len = strlen($startString);
    return (substr($string, 0, $len) === $startString);
}

function parseLine($lineParts) {
    $arrayLength = count($lineParts);
    echo "++ Array length = " . $arrayLength . " ";
    if ($lineParts[0] == "mvi") {
        echo "MVI";
    }
    echo "|";
    for ($i = 0; $i < $arrayLength; $i++) {
        echo $lineParts[$i] . "|";
    }
    echo "\xA";
}

// -----------------------------------------------------------------------------
echo "+++ Start.\xA";

$asmFile = "p1.asm";
echo "+ Read file: " . $asmFile . "\xA";
$readFile = fopen($asmFile, "r");
while (!feof($readFile)) {
    $lineParts = explode(" ", trim(fgets($readFile)));
    if ($lineParts[0] == "") {
        // echo "++ Empty line.";
    } elseif (startsWith($lineParts[0], ";")) {
        // echo "++ Comment line.";
    } else {
        parseLine($lineParts);
    }
    // echo "\xA";
}
fclose($readFile);
echo "+ File read, and closed.\xA";

echo "\xA+++ Exit.\xA";
?>
