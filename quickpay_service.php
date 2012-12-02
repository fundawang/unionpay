<?php

require_once(dirname(__FILE__) . "/quickpay_conf.php");

class quickpay_service
{
	var $gateway;
	var $arr_parameter;
	var $signature;
    var $notifySignature;
    var $notifySignMethod;

	function quickpay_service($arr_parameter, $service = QUICKPAY_PAY_SEVICE)
	{
        $check_arr_parameter = array();
        if ($service == QUICKPAY_PAY_SEVICE) {
            $check_arr_parameter = quickpay_conf::$ARR_PAY_PARAMETER;
            foreach (quickpay_conf::$ARR_SP_CONF as $conf_item_key => $conf_item_value) {
                $this->arr_parameter[$conf_item_key] = $conf_item_value;
            }
        }
        else if ($service == QUICKPAY_NOTIFY_SEVICE) {
            $check_arr_parameter = quickpay_conf::$ARR_NOTIFY_PARAMETER;
            if (!isset($arr_parameter['signature']) || !isset($arr_parameter['signMethod'])) {
                die("notify but without signature or signMethod");
            }
            $this->notifySignature = $arr_parameter['signature'];
            $this->notifySignMethod = $arr_parameter['signMethod'];
            unset($arr_parameter['signature']);
            unset($arr_parameter['signMethod']);
        }
        else {
            die("unsuported service : " . $service);
        }

	    foreach ($check_arr_parameter as $key) {
	        if (!isset($arr_parameter[$key])) {
	            die("parameter [" . $key . "] is not set in input parameters");
	        }
            $this->arr_parameter[$key] = $arr_parameter[$key];
	    }
        $this->sign_type = quickpay_conf::$SIGN_TYPE;
        $this->gateway = quickpay_conf::$GATE_WAY;

        $sort_array = $this->arr_parameter;
		ksort($sort_array);
		reset($sort_array);
		$sign_str = "";
		while (list ($key, $val) = each ($sort_array)) {
			$sign_str .= $key . "=" . $this->charset_encode($val, $this->arr_parameter['charset'], quickpay_conf::$FILE_CHARSET) . "&";
		}

		$this->signature = md5($sign_str . md5(quickpay_conf::$SECURITY_KEY));
	}

    function verify_notify()
    {
        if ($this->notifySignMethod != 'MD5') {
            die("unsuported signMethod: " . $this->notifySignMethod);
        }
        if ($this->signature != $this->notifySignature) {
            return false;
        }

        return true;
    }

	function create_html()
	{
        $html = '<meta http-equiv="Content-Type" content="text/html; charset='.$this->arr_parameter['charset'].'" />';
		$html .= '<script language="javascript">window.onload=function(){document.pay_form.submit();}</script>';
		$html .= '<form id="pay_form" name="pay_form" action="' . $this->gateway . '" method="post">';

        foreach ($this->arr_parameter as $key => $value) {
            $html .= '<input type="hidden" name="' . $key . '" id="' . $key . '" value="' . $this->charset_encode($value, $this->arr_parameter['charset'], quickpay_conf::$FILE_CHARSET) . '" />';
        }
        $html .= '<input type="hidden" name="signature" id="signature" value="' . $this->signature . '">
            <input type="hidden" name="signMethod" id="signMethod" value="' . quickpay_conf::$SIGN_TYPE . '" /></form>';

		return $html;
	}

	function charset_encode($input, $output_charset , $input_charset ="UTF-8")
	{
		if(!isset($output_charset)) {
		    $output_charset  = $this->arr_parameter['charset'];
		}
		if($input_charset == $output_charset || $input ==null) {
			return $input;
		}
		if (function_exists("mb_convert_encoding")) {
			return mb_convert_encoding($input, $output_charset, $input_charset);
		}
		if(function_exists("iconv")) {
			return iconv($input_charset, $output_charset, $input);
		}

		die("sorry, you have no libs support for charset convert.");
	}
}

?>
