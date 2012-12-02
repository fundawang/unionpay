<?php

require_once(dirname(__FILE__) . '/quickpay_service.php');

if ($_SERVER["REQUEST_METHOD"] != 'POST') {
    die("NOTIFY ONLY SUPPORT POST REQUEST");
}

$quickpay_service = new quickpay_service($_POST, QUICKPAY_NOTIFY_SEVICE);
if (!$quickpay_service->verify_notify()) {
    die("upop notify, but verify failed");
}

/* TODO verify sign success, now we may be do the other things */

?>