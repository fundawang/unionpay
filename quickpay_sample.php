<?php

require_once(dirname(__FILE__) . '/quickpay_service.php');

// seed with microseconds
function make_seed()
{
  list($usec, $sec) = explode(' ', microtime());
  return (float) $sec + ((float) $usec * 100000);
}
mt_srand(make_seed());

$parameter['transType']         = '01';
$parameter['origQid']               = '';
$parameter['commodityUrl']      = "http://218.80.192.223/shop1/payment/quickpay/quickpay_result.php?payid=123456";
$parameter['commodityName']     = '亮片刺绣闪耀款短袖T恤';
//$parameter['commodityName']     = 'commodityName';
$parameter['commodityUnitPrice'] = 11000;
$parameter['commodityQuantity'] = 1;
$parameter['commodityDiscount'] = 0;
$parameter['transferFee']       = 1000;
//$parameter['orderNumber']       = 2345671 . time();
$parameter['orderNumber']       = date('YmdHis') . strval(mt_rand(100, 999));
$parameter['orderAmount']       = 11000;
$parameter['orderCurrency']     = '156';
$parameter['orderTime']         = date('YmdHis');
$parameter['customerIp']        = '172.17.136.34';
$parameter['customerName']      = 'customer Name';
$parameter['defaultPayType']    = '';
$parameter['defaultBankNumber'] = '';
$parameter['transTimeout']      = '300000';
$parameter['merReserved']       = '';
$parameter['frontEndUrl']       = "http://218.80.192.223/shop1/payment/quickpay/quickpay_result.php?";
$parameter['backEndUrl']        = "http://218.80.192.223/shop1/payment/quickpay/quickpay_result.php?";

$pay_service = new quickpay_service($parameter);
$html = $pay_service->create_html();

//var_dump($html);
echo $html;

?>
