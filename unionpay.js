function password(length) {
  var iteration = 0;
  var password = "";
  var randomNumber;
  while(iteration < length){
    randomNumber = Math.floor(Math.random()*10);;
    iteration++;
    password += randomNumber.toString();
  }
  return password;
}
