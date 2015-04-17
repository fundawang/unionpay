<?php

class unionpay {
	public function convertParamsToString($params) {
		$sign_str = array();
		ksort ( $params );
		foreach ( $params as $key => $val ) {
			if ($key == 'signature') {
				continue;
			}
			$sign_str[] = sprintf ( "%s=%s", $key, $val );
		}
		return implode('&', $sign_str);
	}
}

class unionpay_frontpay extends unionpay {
	var $params;
	protected $cert_id;
	protected $private_key;

	public function __construct($privatekey_file=NULL, $privatekey_passwd=NULL) {
		if(empty($privatekey_file)) $this->private_key = variable_get('unionpay_privatekey');
		else {
			$pkcs12 = file_get_contents ( $privatekey_file );
			openssl_pkcs12_read ( $pkcs12, $certs, $privatekey_passwd );
			$this->private_key = $certs ['pkey'];
		}

		if(empty($privatekey_passwd)) $this->cert_id = variable_get('unionpay_certid');
		else {
			$certdata = openssl_x509_parse ( $certs ['cert'] );
			$this->cert_id = $certdata ['serialNumber'];
		}

		$this->params=array(
			'version' => '5.0.0',				//版本号
			'encoding' => 'utf-8',				//编码方式
			'certId' => $this->cert_id,			//证书ID
			'txnType' => '01',				//交易类型	
			'txnSubType' => '01',				//交易子类
			'bizType' => '000201',				//业务类型
			'signMethod' => '01',		//签名方法
			'channelType' => '07',		//渠道类型，07-PC，08-手机
			'accessType' => '0',		//接入类型
			'currencyCode' => '156',	//交易币种
		);
	}

	public function getPrivateKey() {
		return $this->private_key;
	}

	public function getCertId() {
		return $this->cert_id;
	}

	private function sign() {
		if(isset($this->params['transTempUrl'])){
			unset($this->params['transTempUrl']);
		}
		$params_str = parent::convertParamsToString($this->params);
		$params_sha1x16 = sha1 ( $params_str, FALSE );
		$sign_flag = openssl_sign ( $params_sha1x16, $signature, $this->private_key, OPENSSL_ALGO_SHA1 );
		if ($sign_flag) {
			$signature_base64 = base64_encode ( $signature );
			$this->params ['signature'] = $signature_base64;
		}
	}

	public function setParams($params) {
		foreach($params as $key => $value) {
			$this->params[$key]=$value;
		}
	}

	public function __toString() {
		if(!isset($this->params['signature'])) $this->sign();

		$site_status=variable_get('unionpay_site_status', 'test');

		switch($site_status) {
			case 'test':
				$action='https://101.231.204.80:5000/gateway/api/frontTransReq.do';
				break;
			case 'production':
				$action='https://gateway.95516.com/gateway/api/frontTransReq.do';
				break;
		}

		$html = <<<eot
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	</head>
	<body onload="javascript:document.pay_form.submit();">
		<form id="pay_form" name="pay_form" action="{$action}" method="post">

eot;
		foreach ( $this->params as $key => $value ) {
			$html .= "			<input type=\"hidden\" name=\"{$key}\" id=\"{$key}\" value=\"{$value}\" />\n";
		}
		$html .= <<<eot
		</form>
	</body>
	</html>
eot;
		return $html;
	}
}

class unionpay_response extends unionpay {
	var $params;

	public function __construct($publickey_file, $params) {
		if(empty($publickey_file))
			$public_key = file_get_contents ( DRUPAL_ROOT . '/' . drupal_get_path('module', 'unionpay') . '/verify_sign_acp.cer' );
		else
			$public_key = $publickey_file;
		$signature_str = $params ['signature'];
		unset ( $params ['signature'] );

		$params_str = parent::convertParamsToString ( $params );
		$signature = base64_decode ( $signature_str );
		$params_sha1x16 = sha1 ( $params_str, FALSE );
		$isSuccess = openssl_verify ( $params_sha1x16, $signature,$public_key, OPENSSL_ALGO_SHA1 );
		if(!$isSuccess)
			throw new Exception('Invalid input');
		else
			$this->params=$params;
	}
}

class unionpay_query extends unionpay {
	var $params;
	protected $cert_id;
	protected $private_key;

	public function __construct($privatekey_file=NULL, $privatekey_passwd=NULL) {
		if(empty($privatekey_file)) $this->private_key = variable_get('unionpay_privatekey');
		else {
			$pkcs12 = file_get_contents ( $privatekey_file );
			openssl_pkcs12_read ( $pkcs12, $certs, $privatekey_passwd );
			$this->private_key = $certs ['pkey'];
		}

		if(empty($privatekey_passwd)) $this->cert_id = variable_get('unionpay_certid');
		else {
			$certdata = openssl_x509_parse ( $certs ['cert'] );
			$this->cert_id = $certdata ['serialNumber'];
		}

		$this->params = array(
			'version' => '5.0.0',		//版本号
			'encoding' => 'utf-8',		//编码方式
			'certId' => $this->cert_id,	//证书ID	
			'signMethod' => '01',		//签名方法
			'txnType' => '00',		//交易类型	
			'txnSubType' => '00',		//交易子类
			'bizType' => '000000',		//业务类型
			'accessType' => '0',		//接入类型
			'channelType' => '07',		//渠道类型
		);
	}

	private function sign() {
		if(isset($this->params['transTempUrl'])){
			unset($this->params['transTempUrl']);
		}
		$params_str = parent::convertParamsToString($this->params);
		$params_sha1x16 = sha1 ( $params_str, FALSE );
		$sign_flag = openssl_sign ( $params_sha1x16, $signature, $this->private_key, OPENSSL_ALGO_SHA1 );
		if ($sign_flag) {
			$signature_base64 = base64_encode ( $signature );
			$this->params ['signature'] = $signature_base64;
		}
	}

	public function setParams($params) {
		foreach($params as $key => $value) {
			$this->params[$key]=$value;
		}
	}

	public function execute() {
		if(!isset($this->params['signature'])) $this->sign();

		$site_status=variable_get('unionpay_site_status', 'test');

		switch($site_status) {
			case 'test':
				$action='https://101.231.204.80:5000/gateway/api/queryTrans.do';
				break;
			case 'production':
				$action='https://gateway.95516.com/gateway/api/queryTrans.do';
				break;
		}

		$opts=drupal_http_build_query($this->params);

		$result=drupal_http_request($action, array(
			'method'=>'POST',
			'headers'=>array('Content-type'=>'application/x-www-form-urlencoded;charset=UTF-8'),
			'data'=>$opts,
		));
		$data=$result->data;

		try {
			$response = new unionpay_response(variable_get('unionpay_pubkey_for_unionpay'), drupal_get_query_array($data));
		} catch(Exception $exp) {
			watchdog('unionpay',
				'Critical error occured: @returnstr.', array(
					'@returnstr' => $exp->getMessage(),
				),
				WATCHDOG_CRITICAL
			);
			return FALSE;
		}

		return $response;
	}
}
?>
