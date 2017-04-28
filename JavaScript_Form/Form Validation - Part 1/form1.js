function validateForms() {
  var userId = document.signup.userId;
  var password = document.signup.passwordId;
  var userName = document.signup.username;
  var userAddress = document.signup.address;
  var userZip = document.signup.zip;
  var userEmail = document.signup.email;
  var maleGender = document.signup.mgender;
  var femaleGender = document.signup.fgender;
    
  if(validateUserId(userId, 6, 15)){
    if(validatePasswordId(password, 8, 15)){
      if(validateUserName(userName)){
        if(validateAddress(userAddress)){
            
        }
      }    
    }   
  }
  return false;
}

function validateUserId(userId, min, max){
  var userIdLength = userId.value.length;
  if(userIdLength == 0 || userIdLength>=max || userIdLength<min){
      alert("user ID must be between" + min + " to " + max + " characters");
    userId.focus();
    return false;
  }
  return true;
}

function validatePasswordId(passwordId, min, max){
    
    var passwordLength = passwordId.value.length;
    if(passwordLength == 0 || passwordLength >= max || passwordLength < min){
        alert("password length must be between " + min + " to " + max + " characters");
        passwordId.focus();
        return false;
    }
    return true;
}

function validateUserName (userName) {
  var letters = /^[A-Za-z]+$/;
    
  if(userName.value.match(letters)){
      return true;
  }
  else{
      alert("must have alphebet characters only");
      userName.focus();
      return false;
  }
  return true;
}

function validateAddress (userAddress){
    var characters = /^[0-9A-Za-z\s+]+$/;
    if(userAddress.match(characters)){
        return true;
    }
    else{
        alert("User Address must have alphnumeric characters only");
        userAddress.focus();
        return false;
    }
    return true;
}