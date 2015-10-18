/* Bsp. input: 'myjp({"92":22})'
        output "0...100" als String */
input.substring(input.indexOf(":") + 1, input.lastIndexOf("}"))
