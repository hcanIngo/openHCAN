/* Bsp. input: 'myjp({"92":22})'
	input: 0...100        
	output: invertiert 100 ... 0 als String */
100 - (input.substring(input.indexOf(":") + 1, input.lastIndexOf("}")))
//parseInt(100 - (input.substring(input.indexOf(":") + 1, input.lastIndexOf("}"))))
