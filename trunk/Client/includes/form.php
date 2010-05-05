<?php 
 
$values = array();  //Holds submitted form field values
$errors = array();	//Holds submitted form error messages
$num_errors;		//The number of errors in submitted form

if(isset($_SESSION['value_array']) && isset($_SESSION['error_array']))
{
	$values = $_SESSION['value_array'];
	$errors = $_SESSION['error_array'];
	$num_errors = count($errors);

	unset($_SESSION['value_array']);
	unset($_SESSION['error_array']);
}
else
{
	$num_errors = 0;
}

/**
* setValue - Records the value typed into the given
* form field by the user.
*/
function setValue($field, $value)
{
	$values[$field] = $value;
}

/**
* setError - Records new form error given the form
* field name and the error message attached to it.
*/
function setError($field, $errmsg)
{
	$errors[$field] = $errmsg;
	$num_errors = count($errors);
}

/**
* value - Returns the value attached to the given
* field, if none exists, the empty string is returned.
*/
function value($field)
{
	if(array_key_exists($field,$values))
	{
		return htmlspecialchars(stripslashes($values[$field]));
	}
	else
	{
		return "";
	}
}

/**
* error - Returns the error message attached to the
* given field, if none exists, the empty string is returned.
*/
function error($field)
{
	if(array_key_exists($field, $errors))
	{
		return "<font size=\"2\" color=\"#ff0000\">".$errors[$field]."</font>";
	}
	else
	{
		return "";
	}
}

/* getErrorArray - Returns the array of error messages */
function getErrorArray()
{
	return $errors;
}
 
?>
