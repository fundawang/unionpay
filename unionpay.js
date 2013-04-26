function password(length) {
	var iteration = 0;
	var password = "";
	var randomNumber;
	var x="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for(var i=0;i<length;i++)  {
		password += x.charAt(Math.ceil(Math.random()*100000000)%x.length);
	}
	return password;
}
